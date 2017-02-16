package com.example.triviathrowtoy.trivio;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        loginRedirect();
    }

    private void loginRedirect() {
        if(SaveSharedPreferences.getUserName(MainActivity.this).length() == 0)
        {
            // call Login Activity
            Intent intent = new Intent(this, LoginActivity.class);
            startActivity(intent);
        }
        else
        {
            // redirect to main menu
            Intent intent = new Intent(this, MenuActivity.class);
            startActivity(intent);
        }
    }
}
