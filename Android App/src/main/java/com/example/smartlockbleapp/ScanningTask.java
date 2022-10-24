package com.example.smartlockbleapp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.AnimationUtils;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import android.util.Log;


import android.os.Handler;
import android.bluetooth.le.ScanSettings;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;

import android.content.pm.PackageManager;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.le.BluetoothLeScanner;

import android.bluetooth.BluetoothDevice;

public class ScanningTask {

    private Activity act;

    private AlertDialog dialog;
    private ListView BleDeviceList;

    private boolean scanning = false;
    private static final long SCAN_PERIOD = 10000;

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bluetoothLeScanner;
    private ScanSettings scanSettings;

    private LeDeviceListAdapter leDeviceListAdapter;
    private Handler handler;

    ScanningTask(Activity activity, AlertDialog dialog, ListView bleDeviceList){
        this.act = activity;
        this.dialog = dialog;
        this.BleDeviceList = bleDeviceList;
        this.leDeviceListAdapter = new LeDeviceListAdapter(activity);
        handler = new Handler();
    }

    public void scanInit(){
        /* Get a Bluetooth Adapter Object */
        final  BluetoothManager bluetoothManager = (BluetoothManager) act.getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
        bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
        scanSettings = new ScanSettings.Builder().build();
    }

    public void scanLeDevice() {
        if (!scanning) {
            // stop scanning after a predefined scan period
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    scanFinished();
                    bluetoothLeScanner.stopScan(scanCallback);
                    Toast.makeText(act, "scan finished, pls select a bluetooth device", Toast.LENGTH_SHORT).show();
                }
            }, SCAN_PERIOD);

            scanning = true;
            bluetoothLeScanner.startScan(null, scanSettings, scanCallback);
            Toast.makeText(act, "scanning bluetooth devices", Toast.LENGTH_LONG).show();

            addScanningAnimation();

        } else {
            scanning = false;
            bluetoothLeScanner.stopScan(scanCallback);
            Toast.makeText(act, "scan finished, pls select a bluetooth device", Toast.LENGTH_SHORT).show();
        }
    }

    private void scanFinished() {
        scanning = false;
        Log.d("BLEDEVICE", "Scan finished!");

        /* alert number of scanned devices */
        String textData = "";
        int total = leDeviceListAdapter.getCount();
        if (total == 0)
            textData = "NO DEVICE FOUND";
        else if (total == 1)
            textData = "1 DEVICE FOUND";
        else if(total > 1) {
            StringBuilder builder = new StringBuilder();
            builder.append(String.valueOf(total + " DEVICES FOUND"));
            textData = builder.toString();
        }
        ((TextView)dialog.findViewById(R.id.scanning_status)).setText(textData);

        /* map scanned device list into listview */
        BleDeviceList.setAdapter(leDeviceListAdapter);
        BleDeviceList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                BluetoothDevice connectDevice = leDeviceListAdapter.getDevice(i);
                Toast.makeText(act, connectDevice.getName(), Toast.LENGTH_LONG).show();
                listViewOnClickCallBack.connectGattServer(connectDevice);
            }
        });
    }

    private void addScanningAnimation(){
        TextView txt = (TextView)dialog.findViewById(R.id.scanning_status);
        txt.setAnimation(AnimationUtils.loadAnimation(act, R.anim.scanning_animate));
    }

    private ScanCallback scanCallback = new ScanCallback() {

        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);

            BluetoothDevice device = result.getDevice();
            String name = device.getName();
            String address = device.getAddress();
            int rssi = result.getRssi();
            Log.d("BLEDEVICE", "Device: " + name + " " + "Address: " + address + " " + "Rssi: " + rssi);
            if(name != null)  leDeviceListAdapter.addDevice(device);
        }
    };

    public interface ListViewOnClickCallBack{
        public void connectGattServer(BluetoothDevice connectDevice);
    }

    private ListViewOnClickCallBack listViewOnClickCallBack;
    public void SetListViewOnClickCallBack(ListViewOnClickCallBack callBack){
        this.listViewOnClickCallBack = callBack;
    }
}
