package com.example.smartlockbleapp;

import android.content.Context;
import android.widget.BaseAdapter;
import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import java.util.ArrayList;

import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;
import android.view.LayoutInflater;


public class LeDeviceListAdapter extends BaseAdapter {

    private Context context;
    private ArrayList<BluetoothDevice> mLeDevices = new ArrayList<BluetoothDevice>();

    public LeDeviceListAdapter(Context context) {
        this.context = context;
    }

    public void addDevice(BluetoothDevice device) {
        if( !(mLeDevices.contains(device)) ) {
            mLeDevices.add(device);
        }
    }

    public BluetoothDevice getDevice(int position) {
        return mLeDevices.get(position);
    }

    @Override
    public int getCount() {
        return mLeDevices.size();
    }

    @Override
    public Object getItem(int i) {
        return mLeDevices.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }


    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {

        LayoutInflater inflater = (LayoutInflater) this.context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View BleItem = inflater.inflate(R.layout.custom_listview, viewGroup, false);
        TextView BleServiceName = (TextView) BleItem.findViewById(R.id.ble_item);
        BleServiceName.setText(mLeDevices.get(i).getName());
        return BleItem;
    }
}