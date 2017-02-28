package com.example.triviathrowtoy.trivio;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.TextView;

import java.io.IOException;
import java.util.ArrayList;

import static android.net.wifi.SupplicantState.DISCONNECTED;
import static android.net.wifi.SupplicantState.UNINITIALIZED;

public class FactHistoryActivity extends AppCompatActivity {

    private ListView factHistoryList;
    private ArrayList<FactItem> factHistory = new ArrayList<FactItem>();
    private String getRequestUrl = "http://api-env.ptt9xgaruh.us-west-2.elasticbeanstalk.com/";
    private String getRequestId = "1";
    private String FACT_PARCEL = "PARCELABLE_FACT";
    private View mProgressView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fact_history);

        factHistoryList = (ListView)findViewById(R.id.historyListView);

        mProgressView = findViewById(R.id.fact_progress);
        showProgress(true);

        requestFacts();
    }

    /**
     * Requests fact history from server.
     */
    private void requestFacts() {

        SupplicantState supplicantState;
        WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        supplicantState = wifiInfo.getSupplicantState();

        if(supplicantState == DISCONNECTED || supplicantState == UNINITIALIZED) {
            showProgress(false);
            TextView noFacts = (TextView)findViewById(R.id.noFactsText);
            noFacts.setVisibility(View.VISIBLE);
            return;
        }


        Log.d("ATTEMPT_REQUEST","Attempting get request.");

        String result = "";

        HttpGetRequest getRequest = new HttpGetRequest();
        try {
            result = getRequest.execute(getRequestUrl + getRequestId).get();

        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

        if(!result.isEmpty()) {
            Log.d("GET_REQUEST",result);
            factHistory.add(new FactItem(result, (long)1));
        }

        if(factHistory.size() != 0) {
            setListView();
        }
        else {
            TextView noFacts = (TextView)findViewById(R.id.noFactsText);
            noFacts.setVisibility(View.VISIBLE);
        }

        showProgress(false);
    }

    private void setListView() {
        factHistoryList.setAdapter(new FactHistoryListAdapter(this, R.layout.fact_history_item,factHistory));

        factHistoryList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position,
                                    long id) {
                Intent intent = new Intent(FactHistoryActivity.this, ViewFactActivity.class);
                FactItem factItem = (FactItem) factHistoryList.getItemAtPosition(position);
                intent.putExtra(FACT_PARCEL, factItem);
                startActivity(intent);
            }
        });
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
