package com.example.triviathrowtoy.trivio;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

public class ViewFactActivity extends AppCompatActivity {

    private String FACT_PARCEL = "PARCELABLE_FACT";
    private String userId;
    private FactItem factItem;
    private static String PutRequestUrl = "http://triviotoy.azurewebsites.net/trivia/vote";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_fact);

        Bundle bundle = getIntent().getExtras();
        factItem = bundle.getParcelable(FACT_PARCEL);

        //userId = SaveSharedPreferences.getUserID(this);
        userId = "4";

        if(factItem!= null) {
            TextView factTextView = (TextView)findViewById(R.id.factText);
            factTextView.setText(factItem.getFactString());
        }

        setVoteButtons();
    }

    private void setVoteButtons() {
        final ImageButton downVoteButton = (ImageButton) findViewById(R.id.downVoteButton);
        final ImageButton upVoteButton = (ImageButton) findViewById(R.id.upVoteButton);

        if(factItem.getVoteStatus().equals(FactItem.voteStatuses.notVoted.toString())) {
            upVoteButton.setImageResource(R.drawable.ic_action_thumbs_up);
            downVoteButton.setImageResource(R.drawable.ic_action_thumbs_down);
        }
        else if(factItem.getVoteStatus().equals(FactItem.voteStatuses.downVote.toString())) {
            upVoteButton.setImageResource(R.drawable.ic_action_thumbs_up);
            downVoteButton.setImageResource(R.drawable.ic_action_thumbs_down_light);
        }
        else {
            upVoteButton.setImageResource(R.drawable.ic_action_thumbs_up_light);
            downVoteButton.setImageResource(R.drawable.ic_action_thumbs_down);
        }

        downVoteButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                factItem.setVoteStatus(FactItem.voteStatuses.downVote);
                upVoteButton.setImageResource(R.drawable.ic_action_thumbs_up);
                downVoteButton.setImageResource(R.drawable.ic_action_thumbs_down_light);
                buildJsonString();
                putRequest();
            }
        });

        upVoteButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                factItem.setVoteStatus(FactItem.voteStatuses.upVote);
                upVoteButton.setImageResource(R.drawable.ic_action_thumbs_up_light);
                downVoteButton.setImageResource(R.drawable.ic_action_thumbs_down);
                putRequest();
            }
        });
    }

    private String buildJsonString() {
        String jsonString;
        JSONObject jsonObject = new JSONObject();
        try {
            jsonObject.accumulate("triviaId", Double.parseDouble(factItem.getFactID()));
            jsonObject.accumulate("userId", Double.parseDouble(userId));
            jsonObject.accumulate("voteType", Double.parseDouble(factItem.getVoteStatus()));
        } catch (JSONException e) {
            e.printStackTrace();
        }

        jsonString = jsonObject.toString();

        return jsonString;
    }

    private void putRequest() {
        String jsonString = buildJsonString();
        HttpPutRequest putRequest = new HttpPutRequest();
        try {
            String result = putRequest.execute(PutRequestUrl, jsonString).get();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
