package com.example.triviathrowtoy.trivio;

import android.app.Fragment;
import android.content.Intent;
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

public class FactHistoryActivity extends AppCompatActivity {

    private ListView factHistoryList;
    private ArrayList<FactItem> factHistory = new ArrayList<FactItem>();
    private String getRequestUrl = "http://api-env.ptt9xgaruh.us-west-2.elasticbeanstalk.com/";
    private String getRequestId = "1";
    private String FACT_PARCEL = "PARCELABLE_FACT";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fact_history);

        factHistoryList = (ListView)findViewById(R.id.historyListView);

        requestFacts();
    }

    /**
     * Requests fact history from server.
     */
    private void requestFacts() {
/*
        // generate array for now
        for (int j = 0; j < 5; j++) {
            factHistory.add(new FactItem("Fact " + j, (long)j));
        }*/

        String result = "";

        HttpGetRequest getRequest = new HttpGetRequest();
        try {
            result = getRequest.execute(getRequestUrl + getRequestId).get();

            Log.d("GET_REQUEST",result);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

        if(result != "") {
            factHistory.add(new FactItem(result, (long)1));
        }

        if(factHistory.size() != 0) {
            setListView();
        }
        else {
            TextView noFacts = (TextView)findViewById(R.id.noFactsText);
            noFacts.setVisibility(View.VISIBLE);
        }
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
}
