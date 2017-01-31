package com.example.triviathrowtoy.trivio;

import android.content.Context;
import android.database.DataSetObserver;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Christine on 1/30/2017.
 */

public class FactHistoryListAdapter extends ArrayAdapter<FactItem> {

    private int resource;
    private LayoutInflater inflater;
    private ArrayList<FactItem> factList;

    public FactHistoryListAdapter(Context context, int resourceID, ArrayList<FactItem> mfactList) {
        super(context, resourceID, mfactList);
        resource = resourceID;
        factList = mfactList;
        inflater = LayoutInflater.from(context);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        try {
            FactItem item = getItem(position);
            View v = null;
            if (convertView == null) {
                LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

                v = inflater.inflate(resource, null);

            } else {
                v = convertView;
            }

            TextView description = (TextView) v.findViewById(R.id.factItem);

            description.setText(item.getFactString());

            return v;
        } catch (Exception ex) {
            Log.e("Listview error","error", ex);
            return null;
        }
    }
}
