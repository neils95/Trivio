package com.example.triviathrowtoy.trivio;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.content.Intent;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Build;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.format.Formatter;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;

public class WifiConnectActivity extends AppCompatActivity {

    private String WIFI_PARCEL = "WIFI_PARCEL";
    private ScanResult selectedWifi;
    private EditText passwordInput;

    private View mProgressView;

    private String address;
    private int port;
    private ClientSocketTask clientSocketTask = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wifi_connect);

        Bundle bundle = getIntent().getExtras();
        selectedWifi = bundle.getParcelable(WIFI_PARCEL);
        address = bundle.getString(CheckPhoneConnectionActivity.ADDRESS_PARCEL);
        port = bundle.getInt(CheckPhoneConnectionActivity.PORT_PARCEL);

        mProgressView = findViewById(R.id.connect_toy_progress);

        if(selectedWifi != null) {
            TextView wifiName = (TextView)findViewById(R.id.wifiName);
            wifiName.setText(selectedWifi.SSID);

            if(!checkProtection()) {
                passwordInput = (EditText)findViewById(R.id.wifiPassword);
                passwordInput.setVisibility(View.INVISIBLE);
            }

        }
    }

    public boolean checkProtection() {
        String capabilities = selectedWifi.capabilities;

        if(capabilities.contains("WEP") || capabilities.contains("PSK") || capabilities.contains("EAP")) {
            return true;
        }

        return false;
    }

    /** Called when the user clicks the connect button */
    public void sendConnection(View view) {
        if(checkPhoneConnection()) {
            //String password = passwordInput.getText().toString();
            String SSID = selectedWifi.SSID;
            // open socket to send credentials
            showProgress(true);
            clientSocketTask = new ClientSocketTask(address, port);
        } else {
            // connect error
        }
    }

    private Boolean checkPhoneConnection() {
        // checks if it is connected to phone wifi
        WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
        String ip = Formatter.formatIpAddress(wifiManager.getConnectionInfo().getIpAddress());

        Log.d("IP_ADDRESS",ip);

        String ADDRESS = ip.substring(0,ip.length()-1);
        ADDRESS = ADDRESS + "1";
        Log.d("SERVER_IP_ADDRESS",ADDRESS);
        return address.equals(ADDRESS);
    }

    // establishes socket connection with toy
    public class ClientSocketTask extends AsyncTask<Void,Void,Void> {

        String dstAddress;
        InetAddress inetAddress;
        int dstPort;
        String response = "";
        Socket socket = null;

        ClientSocketTask(String address, int port) {
            dstAddress = address;
            dstPort = port;
        }

        ClientSocketTask(InetAddress inetaddress, int port) {
            inetAddress = inetaddress;
            dstPort = port;
        }

        @Override
        protected Void doInBackground(Void... params) {
            Log.d("ATTEMPT_SOCKET", "Attempting socket connection.");

            try {
                if (dstAddress.isEmpty()) {
                    socket = new Socket(inetAddress, dstPort);
                } else {
                    socket = new Socket(dstAddress, dstPort);
                }

                Log.d("LISTENING_ON_PORT", String.valueOf(socket.getPort()));

                ByteArrayOutputStream byteArrayOutputStream =
                        new ByteArrayOutputStream(1024);
                byte[] buffer = new byte[1024];

                int bytesRead;
                InputStream inputStream;
                inputStream = socket.getInputStream();

                byte[] messageByte = new byte[1000];
                boolean end = false;
                DataInputStream in = new DataInputStream(socket.getInputStream());

                byte[] receivedULBuffer = new byte[8];

                inputStream.read(receivedULBuffer);
                response = new String(receivedULBuffer, "UTF-8");

                Log.d("SOCKET_RESPONSE", response);

                if (!response.isEmpty()) {
                    Log.d("SOCKET_RESPONSE", response);

                    OutputStream outputStream = socket.getOutputStream();
                    Log.d("OUTPUT_STREAM", "Writing to socket output stream.");
                    for(int i = 0; i < 100; i++) {
                        outputStream.write((byte)3);
                        outputStream.flush();
                    }

                    outputStream.close();
                }

                inputStream.close();


            } catch (Exception ex) {
                //
            } finally {
                if (socket != null) {
                    try {
                        socket.close();
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }
            }
            return null;
        }

        @Override
        protected void onCancelled() {
            if (socket != null) {
                try {
                    socket.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
            showProgress(false);
            clientSocketTask = null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            clientSocketTask = null;
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
