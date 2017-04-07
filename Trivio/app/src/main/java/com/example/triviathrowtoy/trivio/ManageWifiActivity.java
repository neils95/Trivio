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
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class ManageWifiActivity extends AppCompatActivity {

    private ListView wifiListView;
    private List<ScanResult> scanResults;
    private WifiReceiver wifiReceiver;
    private WifiManager wifiManager;
    private View mProgressView;
    private ScanResult selectedWifi;

    //Views
    RelativeLayout phoneConnectionActivity;
    RelativeLayout manageWifiActivity;
    RelativeLayout credentialActivity;
    RelativeLayout successActivity;

    // Phone Connection Activity
    private TextView waitingTextView;
    Server server;

    private String userID;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manage_wifi);
        phoneConnectionActivity = (RelativeLayout)findViewById(R.id.phoneConnectionActivity);
        manageWifiActivity = (RelativeLayout)findViewById(R.id.manageWifiActivity);
        credentialActivity = (RelativeLayout)findViewById(R.id.credentialActivity);
        successActivity = (RelativeLayout)findViewById(R.id.successStatusActivity);

        userID = SaveSharedPreferences.getUserID(this);

        getIntent().setAction("Already created");getIntent().setAction("Already created");

        setCheckPhoneConnection();
    }

    public void setCheckPhoneConnection() {
        mProgressView = findViewById(R.id.connect_phone_progress);
        waitingTextView = (TextView)findViewById(R.id.waitingConnection);

        checkPhoneConnection();
    }

    public void setScanWifiActivity(View view) {
        phoneConnectionActivity.setVisibility(View.GONE);
        manageWifiActivity.setVisibility(View.VISIBLE);

        mProgressView = findViewById(R.id.wifi_progress);
        showProgress(true);

        wifiListView = (ListView)findViewById(R.id.wifiListView);

        wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);

        if(!wifiManager.isWifiEnabled()) {
            wifiManager.setWifiEnabled(true);
        }

        wifiReceiver = new WifiReceiver();
        registerReceiver(wifiReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
        getWifi();
    }

    public void setCredentialActivity() {
        manageWifiActivity.setVisibility(View.GONE);
        credentialActivity.setVisibility(View.VISIBLE);
        mProgressView = findViewById(R.id.connect_toy_progress);
    }

    /** Called when the user clicks the connect button */
    public void sendConnection(View view) {
        if(wifiReceiver != null) {
            unregisterReceiver(wifiReceiver);
        }
        showProgress(true);
        EditText mPasswordView;
        mPasswordView = (EditText) findViewById(R.id.wifiPassword);
        String password = mPasswordView.getText().toString();
        server.sendCredentials(userID,selectedWifi.SSID,password);
    }

    private void checkPhoneConnection() {
        showProgress(true);
        server = new Server(this);
    }

    public void setNextButton() {
        Button nextButton = (Button)findViewById(R.id.nextButton);
        waitingTextView.setVisibility(View.INVISIBLE);
        nextButton.setEnabled(true);
        nextButton.setTextColor(getResources().getColor(R.color.colorTextPrimary));
        showProgress(false);
    }

    public void setSuccess(Boolean success) {
        showProgress(false);
        credentialActivity.setVisibility(View.GONE);
        successActivity.setVisibility(View.VISIBLE);
        TextView statusText = (TextView)findViewById(R.id.connectStatus);
        if(success) {
            //statusText.setText(R.string.successful_connect);
            statusText.setText(R.string.sent_credentials);
        }
    }

    @Override
    protected void onPause() {
//        if(wifiReceiver != null) {
//            unregisterReceiver(wifiReceiver);
//        }
        super.onPause();
    }

    @Override
    protected void onResume() {
        registerReceiver(wifiReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));

//        String action = getIntent().getAction();
//        // Prevent endless loop by adding a unique action, don't restart if action is present
//        if(action == null || !action.equals("Already created")) {
//            if(server != null) {
//                server.closeSocket();
//            }
//            Intent intent = new Intent(this, ManageWifiActivity.class);
//            startActivity(intent);
//            finish();
//        }
//        // Remove the unique action so the next time onResume is called it will restart
//        else
//            getIntent().setAction(null);

        super.onResume();
    }

    @Override
    protected void onDestroy() {
        if(server != null) {
            server.closeSocket();
        }
//        if(wifiReceiver != null) {
//            unregisterReceiver(wifiReceiver);
//        }
        super.onDestroy();
    }

    /** Called when the user clicks the connectWifi button */
    public void selectWifiNetwork(View view) {
        if (selectedWifi != null) {
            setCredentialActivity();
        }
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
                                    long id) {
                selectedWifi = (ScanResult) wifiListView.getItemAtPosition(position);
                selectWifiNetwork(view);
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
