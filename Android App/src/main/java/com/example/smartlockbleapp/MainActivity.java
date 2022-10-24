package com.example.smartlockbleapp;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.media.Image;
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
import java.util.Set;
import android.util.Log;
import android.os.ParcelUuid;
import java.lang.reflect.Method;

import android.os.Handler;
import android.bluetooth.le.ScanSettings;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;

import android.content.pm.PackageManager;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.le.BluetoothLeScanner;

import android.bluetooth.BluetoothDevice;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;

public class MainActivity extends AppCompatActivity implements ScanningTask.ListViewOnClickCallBack, BLERemoteDevice.ServerNotifyCallback {

    ListView BleDeviceList;
    AlertDialog dialog;

    private BLERemoteDevice OptimizationApp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /* create dialog for bluetooth device selection */
        open_dialog();

        /* Check to make sure BLE is supported */
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, "BLE Not Supported", Toast.LENGTH_SHORT).show();
            finish();
        } else {
            Toast.makeText(this, "BLE Supported", Toast.LENGTH_SHORT).show();
        }

        /* scanning activity */
        ScanningTask scanningTask = new ScanningTask(MainActivity.this, dialog, BleDeviceList);
        scanningTask.scanInit();
        scanningTask.SetListViewOnClickCallBack(this);
        scanningTask.scanLeDevice();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d("Main activity logs", "onDestroy");
        BluetoothAdapter _BluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        Set<BluetoothDevice> pairedDevices = _BluetoothAdapter.getBondedDevices();

        /* unpair the bonded device */
        for (BluetoothDevice bt : pairedDevices){
            if (bt.getName().contains("Smartlock")){
                unpairDevice(bt);
            }
        }
    }

    private void unpairDevice(BluetoothDevice device){
        try {
            Method m = device.getClass().getMethod("removeBond", (Class[])null);
            m.invoke(device, (Object[]) null);
        } catch (Exception e) {
            Log.d("Main activity logs", e.getMessage());
        }
    }

    public void open_dialog(){
        AlertDialog.Builder builder = new AlertDialog.Builder(this);

        LayoutInflater inflater = (LayoutInflater) this.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View dialogView = inflater.inflate(R.layout.custom_dialog, null);
        BleDeviceList = (ListView) dialogView.findViewById(R.id.ble_device_list);

        builder.setView(dialogView);

        dialog = builder.create();
        dialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        dialog.show();
    }


    public void addTextEffect(){
        TextView textView = (TextView)findViewById(R.id.notifyVal);
        textView.setTextColor(Color.parseColor("#0000ff"));
        textView.setShadowLayer(8, 0.0f, 0.0f, Color.parseColor("#0000ff"));
    }

    public void addButtonEffect(ImageButton button, byte sendingCommand[]){
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                view.setAlpha(1f);
                view.animate().alpha(0.25f).setDuration(500).setListener(null);

                OptimizationApp.writeCharacteristic(sendingCommand);
            }
        });
    }

    @Override
    public void connectGattServer(BluetoothDevice device) {

        /* start pairing process */
        device.createBond();

        /* broadcast setup
        *  gattServerExchangeDataTask() function called when bonding success
        * */
        registerReceiver(BondingBroadcaster, BondStateFilter);

        // attempt to connect here
        OptimizationApp = new BLERemoteDevice();
        OptimizationApp.setListener(this);

        device.connectGatt(getApplicationContext(), false, OptimizationApp);
    }

    IntentFilter BondStateFilter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
    private BroadcastReceiver BondingBroadcaster = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)){
                BluetoothDevice mDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // 3 cases:

                // case1: bonded already
                if (mDevice.getBondState() == BluetoothDevice.BOND_BONDED) {
                    Log.d("BONDING", "BroadcastReceiver: BOND_BONDED");
                    Toast.makeText(MainActivity.this, "bond success", Toast.LENGTH_SHORT).show();

                    gattServerExchangeDataTask();
                }
                // case2: creating a bond
                if (mDevice.getBondState() == BluetoothDevice.BOND_BONDING) {
                    Log.d("BONDING", "BroadcastReceiver: BOND_BONDING");
                    Toast.makeText(MainActivity.this, "bonding", Toast.LENGTH_LONG).show();
                }
                // case3: breaking a bond
                if (mDevice.getBondState() == BluetoothDevice.BOND_NONE){
                    Log.d("BONDING", "BroadcastReceiver: BOND_NONE");
                    Toast.makeText(MainActivity.this, "bond failed", Toast.LENGTH_SHORT).show();
                }
            }
        }
    };

    private void gattServerExchangeDataTask(){
        dialog.dismiss();
        addTextEffect();
        ImageButton unlockButton = (ImageButton) findViewById(R.id.unlock);
        ImageButton lockButton = (ImageButton) findViewById(R.id.lock);

        byte unlockCommand[] = {0x01};
        byte lockCommand[] = {0x00};
        addButtonEffect(unlockButton, unlockCommand);
        addButtonEffect(lockButton, lockCommand);
    }

    @Override
    public void DisplayNotify(String txt) {

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ((TextView) findViewById(R.id.notifyVal)).setText(txt);
            }
        });
    }
}
