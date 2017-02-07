package com.example.triviathrowtoy.trivio;

import android.Manifest;
import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class ManageWifiActivity extends AppCompatActivity {

    private ListView wifiListView;
    private List<ScanResult> scanResults;
    private WifiReceiver wifiReceiver;
    private WifiManager wifiManager;
    private View mProgressView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manage_wifi);

        mProgressView = findViewById(R.id.wifi_progress);
        showProgress(true);

        wifiListView = (ListView)findViewById(R.id.wifiListView);

        wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);

        if(wifiManager.isWifiEnabled() == false) {
            wifiManager.setWifiEnabled(true);
        }

        wifiReceiver = new WifiReceiver();
        registerReceiver(wifiReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
        //wifiManager.startScan();
        getWifi();
    }

    protected void onPause() {
        unregisterReceiver(wifiReceiver);
        super.onPause();
    }

    protected void onResume() {
        registerReceiver(wifiReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
        super.onResume();
    }

    // Broadcast receiver class called its receive method
    // when number of wifi connections changed
    class WifiReceiver extends BroadcastReceiver {

        // This method call when number of wifi connections changed
        public void onReceive(Context c, Intent intent) {

            scanResults = wifiManager.getScanResults();
            if(scanResults.size() != 0) {
                setListView();
            } else {
                TextView no_networks = (TextView)findViewById(R.id.noNetworksText);
                no_networks.setVisibility(View.VISIBLE);
            }

            showProgress(false);
        }

    }

    private void setListView() {
        wifiListView.setAdapter(new WifiListAdapter(this, R.layout.network_item,scanResults));

        wifiListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position,
                                    long id) {/*
                Intent intent = new Intent(FactHistoryActivity.this, ViewFactActivity.class);
                FactItem factItem = (FactItem) factHistoryList.getItemAtPosition(position);
                intent.putExtra(FACT_PARCEL, factItem);
                startActivity(intent);*/
            }
        });
    }

    // call this method only if you are on 6.0 and up, otherwise call doGetWifi()
    private void getWifi() {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED){
            requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                    0x12345);
            //After this point you wait for callback in onRequestPermissionsResult(int, String[], int[]) overriden method

        }else{
            wifiManager.startScan();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == 0x12345) {
            for (int grantResult : grantResults) {
                if (grantResult != PackageManager.PERMISSION_GRANTED) {
                    return;
                }
            }
            getWifi();
        }
    }


        /**
         * Shows the progress UI and hides the login form.
         */
    @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
    private void showProgress(final boolean show) {
        // On Honeycomb MR2 we have the ViewPropertyAnimator APIs, which allow
        // for very easy animations. If available, use these APIs to fade-in
        // the progress spinner.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
            int shortAnimTime = getResources().getInteger(android.R.integer.config_shortAnimTime);

            mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
            mProgressView.animate().setDuration(shortAnimTime).alpha(
                    show ? 1 : 0).setListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
                }
            });
        } else {
            // The ViewPropertyAnimator APIs are not available, so simply show
            // and hide the relevant UI components.
            mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
        }
    }
}
