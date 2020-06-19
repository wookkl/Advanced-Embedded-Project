package com.example.client_phone;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.widget.TextView;

public class ParkingAreaActivity extends AppCompatActivity {

    TextView text1;
    TextView text2;
    TextView text3;
    TextView text4;
    TextView text5;
    TextView text6;
    Intent intent;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_parking_area);

        text1 = (TextView) findViewById(R.id.text1);
        text2 = (TextView) findViewById(R.id.text2);
        text3 = (TextView) findViewById(R.id.text3);
        text4 = (TextView) findViewById(R.id.text4);
        text5 = (TextView) findViewById(R.id.text5);
        text6 = (TextView) findViewById(R.id.text6);
        intent = getIntent();



        DrawParkingArea();
    }
    public void DrawParkingArea(){
        String s = intent.getStringExtra("parking");
        if(s != null){
            switch(s){
                    case "A":
                        text1.setText("A");
                        break;
                    case "B":
                        text1.setText("B");
                        break;
                    case "C":
                        text1.setText("C");
                        break;
                    case "D":
                        text1.setText("D");
                        break;
                    case "E":
                        text1.setText("E");
                        break;
                    case "F":
                        text1.setText("F");
                        break;
            }
        }
    }
}


