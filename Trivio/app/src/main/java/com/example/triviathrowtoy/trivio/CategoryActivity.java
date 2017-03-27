package com.example.triviathrowtoy.trivio;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.ArrayList;

public class CategoryActivity extends AppCompatActivity {

    private ListView categoryListView;
    private ArrayList<String> categoryList = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_category);

        categoryListView = (ListView)findViewById(R.id.categoryListView);

        categoryList.add("Random");
        //categoryList.add("Science");
        //categoryList.add("Sports");
        //categoryList.add("History");

        setListView();
    }

    private void setListView() {
        categoryListView.setAdapter(new CategoryAdapter(this, R.layout.category_item,categoryList));

        categoryListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position,
                                    long id) {
                //TODO post request for category
            }
        });
    }
}
