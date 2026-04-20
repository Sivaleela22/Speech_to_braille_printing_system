package com.example.speech2text

import android.Manifest
import android.annotation.SuppressLint
import android.app.AlertDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.*
import android.speech.RecognitionListener
import android.speech.RecognizerIntent
import android.speech.SpeechRecognizer
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import com.example.speech2text.ui.theme.Speech2TextTheme
import java.util.*
import kotlin.concurrent.thread

class MainActivity : ComponentActivity() {

    // ---------------- SPEECH ----------------
    private var speechRecognizer: SpeechRecognizer? = null
    private lateinit var recognizerIntent: Intent
    private var isListening = false

    // ---------------- BLUETOOTH ----------------
    private val bluetoothAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
    private var bluetoothSocket: BluetoothSocket? = null
    private val sppUUID: UUID =
        UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        requestAllPermissions()
        setupRecognizer()

        setContent {
            Speech2TextTheme {

                val context = LocalContext.current

                var transcription by remember { mutableStateOf("") }
                var brailleText by remember { mutableStateOf("") }
                var connectionStatus by remember { mutableStateOf("Not Connected") }

                Scaffold { padding ->
                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(padding)
                            .padding(16.dp),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {

                        // -------- CONNECT --------
                        Button(
                            onClick = {
                                scanForBluetoothDevices(
                                    context,
                                    onStatus = { connectionStatus = it }
                                )
                            },
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            Text("Select Bluetooth Device")
                        }

                        Spacer(Modifier.height(8.dp))
                        Text(connectionStatus)
                        Spacer(Modifier.height(16.dp))

                        // -------- LIVE TRANSCRIPTION --------
                        OutlinedTextField(
                            value = transcription,
                            onValueChange = { transcription = it },
                            label = { Text("Live Transcription") },
                            modifier = Modifier
                                .fillMaxWidth()
                                .weight(1f)
                        )

                        Spacer(Modifier.height(16.dp))

                        // -------- BRAILLE DISPLAY --------
                        OutlinedTextField(
                            value = brailleText,
                            readOnly = true,
                            onValueChange = {},
                            label = { Text("Braille Output (for verification)") },
                            modifier = Modifier
                                .fillMaxWidth()
                                .weight(1f)
                        )

                        Spacer(Modifier.height(16.dp))

                        // -------- START / STOP --------
                        Row(horizontalArrangement = Arrangement.spacedBy(16.dp)) {
                            Button(onClick = {
                                if (!isListening) {
                                    startListening { transcription = it }
                                    isListening = true
                                }
                            }) {
                                Text("Speak")
                            }

                            Button(onClick = {
                                stopListening()
                                isListening = false
                            }) {
                                Text("Stop")
                            }
                        }

                        Spacer(Modifier.height(16.dp))

                        // -------- CONVERT & SEND --------
                        Button(
                            onClick = {
                                // Show Braille in app
                                brailleText = textToBraille(transcription)

                                // Send ONLY TEXT to Bluetooth device
                                sendTextToBluetooth(transcription)
                            },
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            Text("Convert & Send")
                        }
                    }
                }
            }
        }
    }

    // ---------------- PERMISSIONS ----------------
    private fun requestAllPermissions() {
        val perms = mutableListOf(
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.ACCESS_FINE_LOCATION,
            Manifest.permission.ACCESS_COARSE_LOCATION
        )
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            perms.add(Manifest.permission.BLUETOOTH_SCAN)
            perms.add(Manifest.permission.BLUETOOTH_CONNECT)
        }
        ActivityCompat.requestPermissions(this, perms.toTypedArray(), 101)
    }

    // ---------------- SPEECH SETUP ----------------
    private fun setupRecognizer() {
        if (!SpeechRecognizer.isRecognitionAvailable(this)) return

        speechRecognizer = SpeechRecognizer.createSpeechRecognizer(this)

        recognizerIntent = Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH).apply {
            putExtra(
                RecognizerIntent.EXTRA_LANGUAGE_MODEL,
                RecognizerIntent.LANGUAGE_MODEL_FREE_FORM
            )
            putExtra(RecognizerIntent.EXTRA_LANGUAGE, "en-US")
            putExtra(RecognizerIntent.EXTRA_PARTIAL_RESULTS, true)
        }
    }

    private fun startListening(onResult: (String) -> Unit) {
        var finalText = ""

        speechRecognizer?.setRecognitionListener(object : RecognitionListener {
            override fun onReadyForSpeech(p0: Bundle?) {}
            override fun onBeginningOfSpeech() {}
            override fun onRmsChanged(p0: Float) {}
            override fun onBufferReceived(p0: ByteArray?) {}
            override fun onEndOfSpeech() {}
            override fun onEvent(p0: Int, p1: Bundle?) {}

            override fun onError(error: Int) {
                if (isListening)
                    speechRecognizer?.startListening(recognizerIntent)
            }

            override fun onResults(results: Bundle?) {
                val list =
                    results?.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION)
                if (!list.isNullOrEmpty()) {
                    finalText += " ${list[0]}"
                    onResult(finalText)
                }
                if (isListening)
                    speechRecognizer?.startListening(recognizerIntent)
            }

            override fun onPartialResults(partialResults: Bundle?) {
                val list =
                    partialResults?.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION)
                if (!list.isNullOrEmpty()) {
                    onResult(finalText + " " + list[0])
                }
            }
        })

        speechRecognizer?.startListening(recognizerIntent)
    }

    private fun stopListening() {
        speechRecognizer?.stopListening()
        speechRecognizer?.cancel()
    }

    // ---------------- BLUETOOTH SCAN ----------------
    @SuppressLint("MissingPermission")
    fun scanForBluetoothDevices(
        context: Context,
        onStatus: (String) -> Unit
    ) {
        val adapter = bluetoothAdapter ?: return

        if (!adapter.isEnabled) {
            startActivity(Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE))
            return
        }

        val devices = mutableListOf<BluetoothDevice>()
        val names = mutableListOf<String>()

        val receiver = object : BroadcastReceiver() {
            override fun onReceive(c: Context?, intent: Intent?) {
                if (intent?.action == BluetoothDevice.ACTION_FOUND) {
                    val device =
                        intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                    if (device != null && !devices.contains(device)) {
                        devices.add(device)
                        names.add("${device.name ?: "Unknown"} (${device.address})")
                    }
                }
            }
        }

        registerReceiver(receiver, IntentFilter(BluetoothDevice.ACTION_FOUND))
        adapter.startDiscovery()

        Handler(Looper.getMainLooper()).postDelayed({
            adapter.cancelDiscovery()
            try { unregisterReceiver(receiver) } catch (_: Exception) {}

            if (devices.isEmpty()) {
                Toast.makeText(context, "No devices found", Toast.LENGTH_SHORT).show()
                return@postDelayed
            }

            AlertDialog.Builder(context)
                .setTitle("Select Device")
                .setItems(names.toTypedArray()) { _, index ->
                    connectToDevice(devices[index], context, onStatus)
                }
                .show()

        }, 6000)
    }

    // ---------------- CONNECT ----------------
    @SuppressLint("MissingPermission")
    fun connectToDevice(
        device: BluetoothDevice,
        context: Context,
        onStatus: (String) -> Unit
    ) {
        thread {
            try {
                bluetoothSocket =
                    device.createRfcommSocketToServiceRecord(sppUUID)
                bluetoothAdapter?.cancelDiscovery()
                bluetoothSocket!!.connect()

                runOnUiThread {
                    onStatus("Connected: ${device.name}")
                }

            } catch (e: Exception) {
                runOnUiThread {
                    onStatus("Connection Failed")
                }
            }
        }
    }

    // ---------------- SEND TEXT ----------------
    fun sendTextToBluetooth(text: String) {
        val socket = bluetoothSocket ?: run {
            Toast.makeText(this, "Not connected", Toast.LENGTH_SHORT).show()
            return
        }

        thread {
            try {
                socket.outputStream.write((text + "\n").toByteArray())
                socket.outputStream.flush()
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        speechRecognizer?.destroy()
        bluetoothSocket?.close()
    }
}

// ---------------- BRAILLE CONVERSION ----------------
fun textToBraille(input: String): String {
    val map = mapOf(
        'a' to '⠁','b' to '⠃','c' to '⠉','d' to '⠙','e' to '⠑',
        'f' to '⠋','g' to '⠛','h' to '⠓','i' to '⠊','j' to '⠚',
        'k' to '⠅','l' to '⠇','m' to '⠍','n' to '⠝','o' to '⠕',
        'p' to '⠏','q' to '⠟','r' to '⠗','s' to '⠎','t' to '⠞',
        'u' to '⠥','v' to '⠧','w' to '⠺','x' to '⠭','y' to '⠽',
        'z' to '⠵',' ' to ' '
    )
    return input.lowercase().map { map[it] ?: it }.joinToString("")
}
