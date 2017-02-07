package com.example.triviathrowtoy.trivio;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    private Boolean isLoggedIn = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        loginRedirect();
    }

    private void loginRedirect() {
        if(isLoggedIn) {
            // redirect to main menu
            Intent intent = new Intent(this, MenuActivity.class);
            startActivity(intent);
        }
        else {
            Intent intent = new Intent(this, LoginActivity.class);
            startActivity(intent);
        }
    }
}
