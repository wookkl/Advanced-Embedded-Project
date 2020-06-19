package com.example.client_phone;


import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.speech.tts.TextToSpeech;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

import static android.speech.tts.TextToSpeech.ERROR;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private static int parkedCarNum = 0;
    String info = "";
    private String host = "172.20.10.10";
    private int port = 9999;
    private Socket client_Socket;
    private BufferedReader inputStream;
    private PrintWriter outputStream;

    //TTS---------------
    private TextToSpeech tts;
    //------------------

    TextView view1;
    TextView view2;
    TextView view3;
    TextView connectInfo;
    TextView checkInfo;
    TextView payInfo;


    EditText ipWriteText;
    EditText parkingCheckCarNum;
    EditText payCheckCarNum;

    Button socketConnectButton;
    Button parkingCheckButton;
    Button payCheckButton;
    Button chargeButton;

    Intent intent;

    ArrayList<String> carInfoList = new ArrayList<String>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ActionBar actionBar = getSupportActionBar();
        assert actionBar != null;
        actionBar.setTitle("주차 관리 시스템");

        view1 = (TextView) findViewById(R.id.view1);
        view2 = (TextView) findViewById(R.id.view2);
        view3 = (TextView) findViewById(R.id.view3);
        connectInfo = (TextView) findViewById(R.id.connectInfo);
        checkInfo = (TextView) findViewById(R.id.checkInfo);
        payInfo = (TextView) findViewById(R.id.payInfo);

        ipWriteText = (EditText) findViewById(R.id.ipWriteText);
        parkingCheckCarNum = (EditText) findViewById(R.id.carNumWrite1);
        payCheckCarNum = (EditText)findViewById(R.id.carNumWrite2);

        socketConnectButton = (Button)findViewById(R.id.send);
        parkingCheckButton = (Button)findViewById(R.id.sendCarNum1);
        payCheckButton = (Button)findViewById(R.id.sendCarNum2);
        chargeButton = (Button) findViewById(R.id.chargeButton);

        socketConnectButton.setOnClickListener(this);
        parkingCheckButton.setOnClickListener(this);
        payCheckButton.setOnClickListener(this);
        chargeButton.setOnClickListener(this);

        intent = new Intent(this, ParkingAreaActivity.class);

        //tts-------------------
        tts = new TextToSpeech(this, new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int status) {
                if(status != ERROR){
                    tts.setLanguage(Locale.KOREAN);
                }
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu){
        getMenuInflater().inflate(R.menu.menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item){
        switch(item.getItemId()){
            case R.id.action_btn1:
                startActivity(intent);
            case R.id.action_btn2:
            case R.id.action_btn3:
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
    @SuppressLint("HandlerLeak")
    Handler mainHandler = new Handler(){
        public void handleMessage(Message msg){
            switch(msg.what){
                case 1:
                    connectInfo.setText(msg.obj.toString());
                    break;
            }
        }
    };
    public void Charge(String Num){
        String chargeCheck = isExist(Num);
        Message msg = new Message();
        msg.what = 1;

        if(chargeCheck.equals("조회되지 않는 차량입니다")){
            msg.obj = chargeCheck;
            mainHandler.sendMessage(msg);
            tts.speak(chargeCheck,TextToSpeech.QUEUE_FLUSH,null);
        }
        else {  //9자리 받아옴
            String fee = String.valueOf(pay(chargeCheck.substring(5))); //돈 가져옴
            outputStream.println("$" + Num + fee);      //돈 보냄
            System.out.println("num : " + Num + "fee : " + fee);
            carInfoList.remove(chargeCheck);            //List에 차 번호,구역,등록시간 삭제
            String region = chargeCheck.substring(4,5); //9자리중 구역 추출
            intent.putExtra("parking",region);    //구역 ParkingActivity로 보냄

            tts.speak("정산이 완료되었습니다.",TextToSpeech.QUEUE_FLUSH,null);
            msg.obj = region;
            mainHandler.sendMessage(msg);
        }
    }

    public int pay(String lastTime){
        String currentTime = getTime();
        int currentSecond = Integer.parseInt(currentTime.substring(0,2)) * 60 + Integer.parseInt(currentTime.substring(2));
        int lastSecond = Integer.parseInt(lastTime.substring(0,2)) * 60 +  Integer.parseInt(lastTime.substring(2));

        return (currentSecond - lastSecond) * 10;
    }

    public String isExist(String text){
        boolean flag = false;
        int index = 0;
        String str = "";
        if(carInfoList != null){
            for(int i = 0 ; i < carInfoList.size(); i++){
                String storedCarNUm = carInfoList.get(i).substring(0,4);
                if(text.equals(storedCarNUm)){
                    flag = true;
                    index = i;
                    break;
                }
            }
            if(flag){
                str = carInfoList.get(index);   //9자리 전부보냄
            }
            else{
                str = "조회되지 않는 차량입니다";
            }
        }
        return str;
    }

    public void onDestroy(){
        super.onDestroy();
        try {
            if(client_Socket != null){
                client_Socket.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        if(tts != null) {
            tts.stop();
            tts.shutdown();
        }
    }

    void connect(final String address){
        Thread checkUpdate = new Thread() {
            public void run() {
                boolean registeredFlag = false;

                String ip = address;
                try {
                    client_Socket = new Socket(ip, port);
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
                try {
                    inputStream = new BufferedReader(new InputStreamReader(client_Socket.getInputStream()));
                    outputStream = new PrintWriter(client_Socket.getOutputStream(),true);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                receive();
            }
        };
        checkUpdate.start();
    }


    void receive(){
        while(true){
            try {
                String recv = inputStream.readLine();
                if(recv != null){
                    System.out.println(recv);
                    if(recv.equals("exit")){
                        client_Socket.close();
                        break;
                    }else{
                        System.out.println(recv);
                        Message m = Message.obtain(null,1,recv);
                        mainHandler.sendMessage(m);

                        if(recv.length() == 5){
                            String check = isExist(recv.substring(0,4));

                            if(check.equals("조회되지 않는 차량입니다")){
                                info = recv + getTime();
                                carInfoList.add(info);

                                Message msg = Message.obtain(null, 1, info);
                                mainHandler.sendMessage(msg);
                            }
                            else{
                                tts.speak("이미 등록된 차량입니다",TextToSpeech.QUEUE_FLUSH,null);
                            }
                        }
                        else if(recv.length() == 4)
                        {
                            Charge(recv);
                        }

                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    public String getTime(){
        long now = System.currentTimeMillis();
        Date mDate = new Date(now);
        @SuppressLint("SimpleDateFormat") SimpleDateFormat simpleDate = new SimpleDateFormat("mm:ss");
        return simpleDate.format(mDate).substring(0,2) + simpleDate.format(mDate).substring(3);
    }

    @SuppressLint("SetTextI18n")
    @Override
    public void onClick(View v) {
        switch(v.getId()){
            case R.id.send:
                String ip = ipWriteText.getText().toString();
                //connect(ip);
                //connect("172.20.10.2");
                connect("192.168.0.185");
                break;
            case R.id.sendCarNum1:
                String carNum = parkingCheckCarNum.getText().toString();
                String carNumCheck = isExist(carNum);

                if(carNumCheck.equals("조회되지 않는 차량입니다")){
                    checkInfo.setText(carNumCheck);
                    tts.speak(carNumCheck,TextToSpeech.QUEUE_FLUSH,null);
                }
                else{
                    checkInfo.setText("차량번호 : " + carNum + ", 주차 구역 : " + carNumCheck.substring(4,5));
                    if(carNum.substring(3).equals("2") || carNum.substring(3).equals("4") || carNum.substring(3).equals("5") || carNum.substring(3).equals("9"))
                        tts.speak("차량번호 " + carNum + ". 는." + carNumCheck.substring(4,5) + "구역에 있습니다",TextToSpeech.QUEUE_FLUSH,null);
                    else
                        tts.speak("차량번호 " + carNum + ". 은." + carNumCheck.substring(4,5) + "구역에 있습니다",TextToSpeech.QUEUE_FLUSH,null);
                }
                break;

            case R.id.sendCarNum2:
                String payCarNum = payCheckCarNum.getText().toString();
                String check = isExist(payCheckCarNum.getText().toString());

                if(check.equals("조회되지 않는 차량입니다")){
                    payInfo.setText(check);
                    tts.speak(check,TextToSpeech.QUEUE_FLUSH,null);
                }
                else{
                    check = check.substring(5);
                    int diff = pay(check);
                    payInfo.setText("차량번호 :" + payCarNum + "주차 요금 : " + diff);
                    tts.speak("차량번호 " + payCarNum + "의 주차 요금은 " + diff +" 원 입니다.",TextToSpeech.QUEUE_FLUSH,null);
                }
                break;
            case R.id.chargeButton:
                String chargeNum = payCheckCarNum.getText().toString();
                Charge(chargeNum);
        }
    }

}

