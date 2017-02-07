package com.example.triviathrowtoy.trivio;

import android.content.Context;
import android.net.wifi.ScanResult;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

import static java.security.AccessController.getContext;

/**
 * Created by Christine on 2/5/2017.
 */

public class WifiListAdapter extends ArrayAdapter<ScanResult> {

    private int resource;
    private LayoutInflater inflater;
    private List<ScanResult> wifiList;

    public WifiListAdapter(Context context, int resourceID, List<ScanResult> mWifiList) {
        super(context,resourceID,mWifiList);
        resource = resourceID;
        wifiList = mWifiList;
        inflater = LayoutInflater.from(context);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        try {
            View v = null;
            if (convertView == null) {
                LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

                v = inflater.inflate(resource, null);

            } else {
                v = convertView;
            }

            TextView wifiName = (TextView) v.findViewById(R.id.networkItem);

            wifiName.setText(getItem(position).SSID);

            return v;
        } catch (Exception ex) {
            Log.e("Listview error","error", ex);
            return null;
        }
    }
}
