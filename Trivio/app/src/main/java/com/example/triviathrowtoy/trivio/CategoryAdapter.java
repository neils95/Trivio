package com.example.triviathrowtoy.trivio;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by Christine on 2/6/2017.
 */

public class CategoryAdapter extends ArrayAdapter<String> {

    private int resource;
    private LayoutInflater inflater;
    private ArrayList<String> categoryList;

    public CategoryAdapter(Context context, int resourceID, ArrayList<String> mcategoryList) {
        super(context, resourceID, mcategoryList);
        resource = resourceID;
        categoryList = mcategoryList;
        inflater = LayoutInflater.from(context);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        try {
            View v = null;
            if (convertView == null) {
                LayoutInflater inflater = (LayoutInflater) getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);

                v = inflater.inflate(resource, null);

            } else {
                v = convertView;
            }

            TextView description = (TextView) v.findViewById(R.id.categoryItem);

            description.setText(getItem(position));

            return v;
        } catch (Exception ex) {
            Log.e("Listview error","error", ex);
            return null;
        }
    }
}
