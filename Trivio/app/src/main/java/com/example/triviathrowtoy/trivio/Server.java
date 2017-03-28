package com.example.triviathrowtoy.trivio;

import android.os.CountDownTimer;
import android.os.Handler;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Created by Christine on 2/22/2017.
 */

public class Server {
    private ManageWifiActivity activity;
    private ServerSocket serverSocket;
    private String message = "";
    private static final int socketServerPORT = 8080;

    private String userID = "";
    private String SSID = "";
    private String password = "";
    private Boolean send = false;
    private Boolean pendingCredentials = true;

    public Server(ManageWifiActivity activity) {
        this.activity = activity;
        Thread socketServerThread = new Thread(new SocketServerThread());
        socketServerThread.start();
    }

    public int getPort() {
        return socketServerPORT;
    }

//    public void onDestroy() {
//        if (serverSocket != null) {
//            try {
//                serverSocket.close();
//                Log.d("SERVER_SOCKET", "closed socket.");
//            } catch (IOException e) {
//                // TODO Auto-generated catch block
//                e.printStackTrace();
//            }
//        }
//    }

    public void sendCredentials(String userid, String ssid, String pw) {
        userID = userid;
        SSID = ssid;
        password = pw;
        send = true;
    }

    private class SocketServerThread extends Thread {

        int count = 0;

        @Override
        public void run() {
            try {
                // create ServerSocket using specified port
                serverSocket = new ServerSocket(socketServerPORT);

                Log.d("SOCKET_SERVER", "opened port.");

                while (true) {
                    // block the call until connection is created and return
                    // Socket object
                    Socket socket = serverSocket.accept();
                    Log.d("SOCKET_SERVER", "toy connected at port " + String.valueOf(socket.getPort()));

                    SocketServerReplyThread socketServerReplyThread =
                            new SocketServerReplyThread(socket);
                    socketServerReplyThread.run();

                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }
    }

    private class SocketServerReplyThread extends Thread {

        private Socket hostThreadSocket;
        OutputStream outputStream = null;
        DataInputStream in = null;

        SocketServerReplyThread(Socket socket) {
            hostThreadSocket = socket;
        }

        @Override
        public void run() {


            // send confirmation of socket connection
            String confirmation = "ok";
            try {
                outputStream = hostThreadSocket.getOutputStream();
                PrintStream printStream = new PrintStream(outputStream);
                printStream.print(confirmation);
                printStream.flush();
                Log.d("SOCKET_STREAM", "confirmation sent");

                // Get confirmation connection from toy
                message = getInputStream(hostThreadSocket);

                while(!message.equals("ok")) {
                    message = getInputStream(hostThreadSocket);
                }

                Log.d("SOCKET_SERVER", "received message: " + message);

                if(message.equals("ok")) {
                    Log.d("SOCKET_SERVER", "connection confirmed");
                    activity.runOnUiThread(new Runnable() {

                        @Override
                        public void run() {
                            activity.setNextButton();
                        }
                    });

                    boolean sendCredentials = true;
                    // Waiting on sending credentials
                    while(pendingCredentials) {
                        if(send) {
                            if(sendCredentials) {
                                // Send credentials
                                Log.d("SOCKET_STREAM", "sending credentials.");
                                String msgReply = userID + ":" + SSID + ":" + password;
                                printStream.print(msgReply);
                                printStream.flush();
                                sendCredentials = false;
                            }

                            message = getInputStream(hostThreadSocket);
                            Log.d("WIFI_CONNECTION_RESULT", message);

                            if(message.equals("yes")) {
                                activity.runOnUiThread(new Runnable() {

                                    @Override
                                    public void run() {
                                        pendingCredentials = false;
                                        activity.setSuccess(true);
                                    }
                                });
                            } else if(message.equals("no")){
                                activity.runOnUiThread(new Runnable() {

                                    @Override
                                    public void run() {
                                        pendingCredentials = false;
                                        activity.setSuccess(false);
                                    }
                                });
                            }
                        }
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                //
            }
        }

        boolean wait = true;
        private void setWait(boolean waitSetting) {
            wait = waitSetting;
        }

        private String getInputStream(Socket socket) {
            String input = "";
            setWait(true);

            new Timer().schedule(
                    new TimerTask() {
                        @Override
                        public void run() {
                            setWait(false);
                        }
                    },
                    500
            );

            while(wait) {
                //
            }

            try {
                if(in == null) {
                    in = new DataInputStream(socket.getInputStream());
                }
                byte[] messageByte = new byte[1000];

                int i = 0;
                while(in.available() > 0) {
                    messageByte[i] = in.readByte();
                    char c = (char) messageByte[i];
                    System.out.println("Byte read: " + c);
                    input += String.valueOf(c);
                    i++;
                }

            } catch (IOException e) {
                e.printStackTrace();
            }

            return input;
        }

    }
}
