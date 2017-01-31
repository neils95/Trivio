package com.example.triviathrowtoy.trivio;

/**
 * Created by Christine on 1/30/2017.
 */

public class FactItem {

    private String factString;
    private Boolean isVoted = false;

    public FactItem(String fact, Boolean voted) {
        factString = fact;
        isVoted = voted;
    }

    public String getFactString() {
        return factString;
    }

    public Boolean getIsVoted() {
        return isVoted;
    }
}
