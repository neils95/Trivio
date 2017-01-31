package com.example.triviathrowtoy.trivio;

import android.app.Fragment;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ListView;

import java.io.IOException;
import java.util.ArrayList;

public class FactHistoryActivity extends AppCompatActivity {

    android.app.ActionBar actionBar = getActionBar();

    private ListView factHistoryList;
    private ArrayList<FactItem> factHistory = new ArrayList<FactItem>();
    private String getRequestUrl = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fact_history);

        actionBar.setDisplayHomeAsUpEnabled(true);

        factHistoryList = (ListView)findViewById(R.id.historyListView);
    }

    /**
     * Requests fact history from server.
     */
    private void requestFacts() {

        // generate array for now
        for (int j = 0; j < 5; j++) {
            factHistory.add(new FactItem("Fact " + j, false));
        }

        String result;

        HttpGetRequest getRequest = new HttpGetRequest();
        try {
            result = getRequest.execute(getRequestUrl).get();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

        if(factHistory.size() != 0) {
            setListView();
        }
    }

    private void setListView() {
        factHistoryList.setAdapter(new FactHistoryListAdapter(this, R.layout.fact_history_item,factHistory));
    }
}
