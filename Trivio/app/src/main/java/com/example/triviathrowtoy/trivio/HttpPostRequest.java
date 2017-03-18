package com.example.triviathrowtoy.trivio;

import android.os.AsyncTask;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;

/**
 * Created by Christine on 2/15/2017.
 */

public class HttpPostRequest extends AsyncTask<String, Void, String> {

    public static final String REQUEST_METHOD = "POST";
    public static final int READ_TIMEOUT = 15000;
    public static final int CONNECTION_TIMEOUT = 15000;

    @Override
    protected String doInBackground(String... params) {
        String stringUrl = params[0];
        String jsonObject = params[1];
        String result;
        String inputLine;
        try {
            //Create a URL object holding our url
            URL myUrl = new URL(stringUrl);
            //Create a connection
            HttpURLConnection connection = (HttpURLConnection)
                    myUrl.openConnection();
            //Set methods and timeouts
            connection.setDoOutput(true);
            connection.setDoInput(true);
            connection.setRequestMethod(REQUEST_METHOD);
            connection.setReadTimeout(READ_TIMEOUT);
            connection.setConnectTimeout(CONNECTION_TIMEOUT);
            connection.setRequestProperty("Content-Type", "application/json");

            //Connect to our url
            connection.connect();

            // Create output stream for JSON object
            OutputStreamWriter out = new OutputStreamWriter(connection.getOutputStream());
            out.write(jsonObject);// here i sent the parameter
            out.close();

            //Create a new buffered reader and String Builder
            StringBuilder stringBuilder = new StringBuilder();

            int HttpResult = connection.getResponseCode();
            if (HttpResult < 400) {

                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(
                        connection.getInputStream(), "utf-8"));

                String readLine = null;

                while ((readLine = bufferedReader.readLine()) != null) {
                    stringBuilder.append(readLine + "\n");
                }

                bufferedReader.close();
                Log.e("Post result", "" + stringBuilder.toString());
                return stringBuilder.toString();
            } else {
                Log.e(" ", "" + connection.getResponseMessage());

                //Set our result equal to our stringBuilder
                BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(
                        connection.getErrorStream(), "utf-8"));

                String readLine = null;

                while ((readLine = bufferedReader.readLine()) != null) {
                    stringBuilder.append(readLine + "\n");
                }

                bufferedReader.close();
                Log.e("Post result", "" + stringBuilder.toString());

                result = stringBuilder.toString();
            }

        } catch (IOException e) {
            e.printStackTrace();
            result = null;
        }

        return result;
    }

    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);
    }
}
