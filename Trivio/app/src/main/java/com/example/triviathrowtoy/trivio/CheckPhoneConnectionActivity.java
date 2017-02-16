package com.example.triviathrowtoy.trivio;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Build;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.format.Formatter;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.charset.StandardCharsets;

public class CheckPhoneConnectionActivity extends AppCompatActivity {

    private Boolean phoneConnected = false;
    private View mProgressView;
    private TextView waitingTextView;
    private int PORT = 333;
    private String ADDRESS = "192.168.4.1";
    private ClientSocketTask clientSocketTask = null;

    public static String ADDRESS_PARCEL = "ADDRESS_PARCEL";
    public static String PORT_PARCEL = "PORT_PARCEL";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_check_phone_connection);

        mProgressView = findViewById(R.id.connect_phone_progress);
        waitingTextView = (TextView)findViewById(R.id.waitingConnection);

        setNextButton();
        checkPhoneConnection();
    }

    private void setNextButton() {
        Button nextButton = (Button)findViewById(R.id.nextButton);
        if( phoneConnected ) {
            nextButton.setEnabled(true);
            nextButton.setTextColor(getResources().getColor(R.color.colorTextPrimary));
            showProgress(false);
        } else {
            nextButton.setEnabled(false);
//            if(clientSocketTask == null) {
//                checkPhoneConnection();
//            }
        }
    }

    protected void onResume() {
        super.onResume();
        setNextButton();
        if(clientSocketTask == null && !phoneConnected) {
            checkPhoneConnection();
        }
    }

    protected void onPause() {
        super.onPause();
        if(clientSocketTask != null) {
            clientSocketTask.cancel(true);
        }
    }

    protected void onStop() {
        super.onStop();
        if(clientSocketTask != null) {
            clientSocketTask.cancel(true);
        }
    }

    private void checkPhoneConnection() {
        showProgress(true);
        String hostname = null;
        try {
            WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
            String ip = Formatter.formatIpAddress(wifiManager.getConnectionInfo().getIpAddress());

            Log.d("IP_ADDRESS",ip);

            ADDRESS = ip.substring(0,ip.length()-1);
            ADDRESS = ADDRESS + "1";
            Log.d("SERVER_IP_ADDRESS",ADDRESS);

            clientSocketTask = new ClientSocketTask(ADDRESS,PORT);
            clientSocketTask.execute();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /** Called when the user clicks the next button */
    public void goToManageWifi(View view) {
        if (phoneConnected) {
            // start intent for manage wifi
            Intent intent = new Intent(this, ManageWifiActivity.class);
            intent.putExtra(ADDRESS_PARCEL, ADDRESS);
            intent.putExtra(PORT_PARCEL,PORT);
            startActivity(intent);
        }
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

//                while(!response.equals("a"))
//                {
//                    bytesRead = in.read(messageByte);
//                    response += new String(messageByte, 0, bytesRead);
//                }

                long pulseLength = 0;

                byte[] receivedULBuffer = new byte[8];

                inputStream.read(receivedULBuffer);

//                for (int i = 0; i < receivedULBuffer.length; i++) {
//
//                    pulseLength += ((long) receivedULBuffer[i] & 0xffL) << (8 * i);
//                }

                response = new String(receivedULBuffer, "UTF-8");

                Log.d("SOCKET_RESPONSE", response);



//                /*
//                 * notice:
//                 * inputStream.read() will block if no data return
//                 */
//                while ((bytesRead = inputStream.read(buffer)) != -1) {
//                    byteArrayOutputStream.write(buffer, 0, bytesRead);
//                    response += byteArrayOutputStream.toString("UTF-8");
//
//                    byte[] responseArray = byteArrayOutputStream.toByteArray();
//
//                    if(response.equals("a")) {
//                        break;
//                    }
//                }

                if (!response.isEmpty()) {
                    Log.d("SOCKET_RESPONSE", response);
                    phoneConnected = true;

                    OutputStream outputStream = socket.getOutputStream();
                    Log.d("OUTPUT_STREAM", "Writing to socket output stream.");
                    for(int i = 0; i < 100; i++) {
                        outputStream.write((byte)12);
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
                    Log.d("CLOSE_SOCKET", "Closing socket.");
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }

            clientSocketTask = null;
        }

        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            setNextButton();
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
            waitingTextView.setVisibility(show ? View.VISIBLE : View.GONE);
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
            waitingTextView.setVisibility(show ? View.VISIBLE : View.GONE);
        }
    }

}
