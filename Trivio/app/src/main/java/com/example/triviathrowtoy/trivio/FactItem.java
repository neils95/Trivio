package com.example.triviathrowtoy.trivio;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Created by Christine on 1/30/2017.
 */

public class FactItem implements Parcelable {

    public enum voteStatuses {
        notVoted("NOT_VOTED"),
        upVote("UPVOTE"),
        downVote("DOWNVOTE");

        private final String status;

        private voteStatuses(String s) {
            status = s;
        }

        public boolean equalsStatus(String otherStatus) {
            return status.equals(otherStatus);
        }

        public String toString() {
            return this.status;
        }
    }

    private String factString;
    private String factID;
    private String voteStatus = voteStatuses.notVoted.toString();

    public FactItem(String fact, String factId) {
        factString = fact;
        factID = factId;
        voteStatus = voteStatuses.notVoted.toString();
    }

    public FactItem(String fact, voteStatuses status, String factId) {
        factString = fact;
        factID = factId;
        voteStatus = status.toString();
    }

    public String getFactString() {
        return factString;
    }

    public String getFactID() {
        return factID;
    }

    public String getVoteStatus() {
        return voteStatus;
    }

    public void setVoteStatus(voteStatuses status) {
        voteStatus = status.toString();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(factString);
        dest.writeString(factID);
        dest.writeString(voteStatus);
    }


    protected FactItem(Parcel in) {
        factString = in.readString();
        factID = in.readString();
        voteStatus = in.readString();
    }

    public static final Creator<FactItem> CREATOR = new Creator<FactItem>() {
        @Override
        public FactItem createFromParcel(Parcel in) {
            return new FactItem(in);
        }

        @Override
        public FactItem[] newArray(int size) {
            return new FactItem[size];
        }
    };
}
