package com.example.triviathrowtoy.trivio;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;

public class ViewFactActivity extends AppCompatActivity {

    private String FACT_PARCEL = "PARCELABLE_FACT";
    private FactItem factItem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_view_fact);

        Bundle bundle = getIntent().getExtras();
        factItem = bundle.getParcelable(FACT_PARCEL);

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
            }
        });

        upVoteButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v)
            {
                factItem.setVoteStatus(FactItem.voteStatuses.upVote);
                upVoteButton.setImageResource(R.drawable.ic_action_thumbs_up_light);
                downVoteButton.setImageResource(R.drawable.ic_action_thumbs_down);
            }
        });
    }
}
