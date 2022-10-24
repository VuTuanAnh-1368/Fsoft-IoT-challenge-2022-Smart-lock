package com.example.smartlockbleapp;

import android.app.Activity;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.content.Context;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.UUID;

import java.util.*;

public class BLERemoteDevice extends BluetoothGattCallback {
    private String tag = "BLEDEVICE";

    private BluetoothGattCharacteristic deviceCharacteristic;
    private BluetoothGatt gattObject;

    private String messageFromServer;

    public BLERemoteDevice() {

    }

    public void writeCharacteristic(byte command[]) {

        BluetoothGattCharacteristic lockState = this.deviceCharacteristic;

        lockState.setValue(command);
        Log.d(tag, "value is " + lockState.getValue()[0]);
        this.gattObject.writeCharacteristic(lockState);
    }

    @Override
    public void onConnectionStateChange (BluetoothGatt gatt, int status, int newState) {
        Log.d(tag,"onConnectionStatChange [" + status + "][" + newState  + "]");
        if (status == BluetoothGatt.GATT_SUCCESS) {
            if (newState == BluetoothGatt.STATE_CONNECTED) {
                Log.d(tag,"Connected to [" + gatt.toString() + "]");
                gatt.discoverServices();
            } else if (status == BluetoothGatt.STATE_DISCONNECTED) {
                Log.d(tag, "Disconnected");
            }
        }
    }

    @Override
    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
        Log.d(tag,"OnServiceDiscovered ["+ status + "] " + gatt.toString());
        List<BluetoothGattService> services = gatt.getServices();
        for (int i = 0; i < services.size(); i++) {
            Log.d(tag, "service [" + i + "] is [" + services.get(i).getUuid().toString() + "]");

            UUID serviceUUID = services.get(i).getUuid();
            List<BluetoothGattCharacteristic> schars = services.get(i).getCharacteristics();
            for (int j = 0; j < schars.size(); j++) {
                Log.d(tag, "characteristic [" + j + "] [" + schars.get(j).getUuid() + "] properties [" + schars.get(j).getProperties() + "]");

                /* "persons present counted" characteristic */
                if (i == 3 && j == 0) {
                    BluetoothGattCharacteristic stateButton = schars.get(j);
                    gatt.setCharacteristicNotification(stateButton, true);

                    List<BluetoothGattDescriptor> desc_s = stateButton.getDescriptors();
                    BluetoothGattDescriptor desc = desc_s.get(0);
                    Log.d(tag, "Descriptor is " + desc.getUuid().toString()); // this is not null
                    desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                    Log.d(tag, "Descriptor write: " + gatt.writeDescriptor(desc)); // returns true

                }

                /* "lockstate" characteristic */
                if(i == 3 && j == 1) {
                    BluetoothGattCharacteristic lockState = schars.get(j);

                    this.deviceCharacteristic = lockState;
                    this.gattObject = gatt;
                }

                List<BluetoothGattDescriptor> scdesc = schars.get(j).getDescriptors();
                for (int k = 0; k < scdesc.size(); k++) {
                    Log.d(tag, "Descriptor [" + k + "] [" + scdesc.get(k).toString() + "]");
                    Log.d(tag, "Descriptor UUID [" + scdesc.get(k).getUuid() + "]");
                    Log.d(tag, "Descriptor Permissions [" + scdesc.get(k).getPermissions() + "]");
                }
            }
        }
    }

    @Override
    public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
        Log.d(tag,"onCharacteristicChanged " + characteristic.getUuid());
        Log.d(tag, "onCharacteristicChanged value = " + characteristic.getValue()[0]);

        int retNumber = characteristic.getValue()[0];
        String textData = "Number of persons present in the room: ";
        StringBuilder builder = new StringBuilder();
        builder.append(textData + String.valueOf(retNumber));
        textData = builder.toString();
        serverNotifyCallback.DisplayNotify(textData);

    }

    public interface ServerNotifyCallback{
        void DisplayNotify(String textData);
    }
    private ServerNotifyCallback serverNotifyCallback;
    public void setListener(ServerNotifyCallback listener){
        this.serverNotifyCallback = listener;
    }

    @Override
    public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
        super.onCharacteristicRead(gatt, characteristic, status);
    }
}
