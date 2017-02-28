package com.example.triviathrowtoy.trivio;

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

/**
 * Created by Christine on 2/22/2017.
 */

public class Server {
    private ManageWifiActivity activity;
    private ServerSocket serverSocket;
    private String message = "";
    private static final int socketServerPORT = 8080;

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

    public void sendCredentials(String ssid, String pw) {
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

        SocketServerReplyThread(Socket socket) {
            hostThreadSocket = socket;
        }

        @Override
        public void run() {
            OutputStream outputStream = null;


            // send confirmation of socket connection
            String confirmation = "ok";
            try {
                outputStream = hostThreadSocket.getOutputStream();
                PrintStream printStream = new PrintStream(outputStream);
                printStream.print(confirmation);
                //printStream.close();
                Log.d("SOCKET_STREAM", "confirmation sent");

                // Get confirmation connection from toy
                message = getInputStream(hostThreadSocket);

                Log.d("SOCKET_SERVER", "received message: " + message);

                if(message.equals("ok")) {
                    Log.d("SOCKET_SERVER", "connection confirmed");
                    activity.runOnUiThread(new Runnable() {

                        @Override
                        public void run() {
                            activity.setNextButton();
                        }
                    });

                    // Waiting on sending credentials
                    while(pendingCredentials) {
                        if(send) {
                            // Send credentials
                            Log.d("SOCKET_STREAM", "sending credentials.");
                            String msgReply = SSID + ":" + password;
                            outputStream = hostThreadSocket.getOutputStream();
                            printStream = new PrintStream(outputStream);
                            printStream.print(msgReply);

                            message = getInputStream(hostThreadSocket);
                            Log.d("WIFI_CONNECTION_RESULT", message);

                            if(message.equals("Success")) {
                                activity.runOnUiThread(new Runnable() {

                                    @Override
                                    public void run() {
                                        activity.setSuccess(true);
                                    }
                                });
                            } else {
                                activity.runOnUiThread(new Runnable() {

                                    @Override
                                    public void run() {
                                        activity.setSuccess(false);
                                    }
                                });
                            }
                            pendingCredentials = false;
                        }
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        private String getInputStream(Socket socket) {
            String input = "";
            boolean end = false;
            DataInputStream in = null;
            try {
                in = new DataInputStream(socket.getInputStream());
                int bytesRead = 0;
                byte[] messageByte = new byte[1000];

                messageByte[0] = in.readByte();
                messageByte[1] = in.readByte();
                ByteBuffer byteBuffer = ByteBuffer.wrap(messageByte, 0, 2);

                int bytesToRead = byteBuffer.getShort();
                System.out.println("About to read " + bytesToRead + " octets");

                // Read from socket
//                in.readFully(messageByte, 0, bytesToRead);

                while(!end)
                {
                    bytesRead = in.read(messageByte);
                    System.out.println("BytesRead: " + bytesRead);
                    input += new String(messageByte, 0, bytesRead);
                    if (input.length() == bytesToRead )
                    {
                        end = true;
                    }
                }

                input = new String(messageByte, 0, bytesToRead);
            } catch (IOException e) {
                e.printStackTrace();
            }
            finally {
                if(in != null) {
                    try {
                        in.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }

            return input;
        }

    }
}
