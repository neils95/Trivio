package com.example.triviathrowtoy.trivio;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MenuActivity extends AppCompatActivity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_menu);
    }

    /** Called when the user clicks the manageWifi button */
    public void startManageWifiIntent(View view) {
        Intent intent = new Intent(this, ManageWifiActivity.class);
        startActivity(intent);
    }

    /** Called when the user clicks the viewFactHistory button */
    public void startFactHistoryIntent(View view) {
        Intent intent = new Intent(this, FactHistoryActivity.class);
        startActivity(intent);
    }

    /** Called when the user clicks the pickCategory button */
    public void startPickCategoryIntent(View view) {
        Intent intent = new Intent(this, CategoryActivity.class);
        startActivity(intent);
    }

    /** Called when the user clicks the Logout button */
    public void userLogout(View view) {
        SaveSharedPreferences.clearPreferences(this);
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
    }
}
