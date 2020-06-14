package com.example.client_phone;


import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.NetworkOnMainThreadException;
import android.speech.tts.TextToSpeech;
import android.text.InputType;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import java.io.BufferedReader;
import java.io.DataInputStream;
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

        socketConnectButton.setOnClickListener(this);
        parkingCheckButton.setOnClickListener(this);
        payCheckButton.setOnClickListener(this);

        //add 하는 부분은 따로 스레드를 생성해서 라즈베리에서 오는 문자열을 계속 수신 -> ArrayList에 저장.
//        carInfoList.add("2222B");
//        carInfoList.add("3333C");

//        long now = System.currentTimeMillis();
//        Date mDate = new Date(now);
//        @SuppressLint("SimpleDateFormat") SimpleDateFormat simpleDate = new SimpleDateFormat("mm:ss");
//        String getTime = simpleDate.format(mDate).substring(0,2) + simpleDate.format(mDate).substring(3);
//
//        info = "1111A" + getTime;
//        carInfoList.add(info);

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
            case R.id.action_btn2:
            case R.id.action_btn3:
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @SuppressLint("SetTextI18n")
    @Override
    public void onClick(View v) {
        switch(v.getId()){
            case R.id.send:
                String ip = ipWriteText.getText().toString();
                //connect(ip);
                connect("172.20.10.2");
                break;
            case R.id.sendCarNum1:
                boolean carCheckFlag = false;
                int index = 0;
                String carNum = parkingCheckCarNum.getText().toString();

                if(carInfoList != null){            //차량번호 앞에 공백문자 들어와서 제거 해줘야함.
                    connectInfo.setText(carInfoList.get(0).substring(0,4));
                    connectInfo.append("  " + carInfoList.get(0).length());
                    for(int i = 0 ; i < carInfoList.size(); i++){
                        String storedCarNUm = carInfoList.get(i).substring(0,4);
                        if(carNum.equals(storedCarNUm)){
                            carCheckFlag = true;
                            index = i;
                        }
                    }
                }
                if(carCheckFlag){
                    //TTS
                    checkInfo.setText("차량번호 : " + carNum + ", 주차 구역 : " + carInfoList.get(index).substring(4,5));
                    if(carNum.substring(3).equals("2") || carNum.substring(3).equals("4") || carNum.substring(3).equals("5") || carNum.substring(3).equals("9"))
                        tts.speak("차량번호 " + carNum + ". 는." + carInfoList.get(index).substring(4,5) + "구역에 있습니다",TextToSpeech.QUEUE_FLUSH,null);
                    else
                        tts.speak("차량번호 " + carNum + ". 은." + carInfoList.get(index).substring(4,5) + "구역에 있습니다",TextToSpeech.QUEUE_FLUSH,null);
                }
                else{
                    //TTS
                    checkInfo.setText("조회되지 않는 차량입니다");
                    tts.speak("조회되지 않는 차량입니다",TextToSpeech.QUEUE_FLUSH,null);
                }
                break;

            case R.id.sendCarNum2:
                boolean payCarCheckFlag = false;
                int payIndex = 0;
                String payCarNum = payCheckCarNum.getText().toString();

                if(carInfoList != null){
                    for(int i = 0 ; i < carInfoList.size(); i++){
                        String storedCarNUm = carInfoList.get(i).substring(0,4);
                        if(payCarNum.equals(storedCarNUm)){
                            payCarCheckFlag = true;
                            payIndex = i;
                        }
                    }
                }
                if(payCarCheckFlag){ //안에 있으면
                    //TTS
                    long now = System.currentTimeMillis();
                    Date mDate = new Date(now);
                    @SuppressLint("SimpleDateFormat") SimpleDateFormat simpleDate = new SimpleDateFormat("mm:ss");
                    String getTime = simpleDate.format(mDate).substring(0,2) + simpleDate.format(mDate).substring(3);

                    int diff = (Integer.parseInt(getTime) - Integer.parseInt(carInfoList.get(payIndex).substring(5))) * 10;
                    payInfo.setText("차량번호 :" + payCarNum + "주차 요금 : " + diff);
                    tts.speak("차량번호 " + payCarNum + "의 주차 요금은 " + diff +" 원 입니다.",TextToSpeech.QUEUE_FLUSH,null);
                }
                else{
                    //TTS
                    payInfo.setText("조회되지 않는 차량입니다");
                    tts.speak("조회되지 않는 차량입니다",TextToSpeech.QUEUE_FLUSH,null);
                }
                break;
        }
    }

    @SuppressLint("HandlerLeak")
    Handler mainHandler = new Handler(){
        public void handleMessage(Message msg){
            switch(msg.what){
                case 1:
                    connectInfo.setText(msg.obj.toString());
                    checkInfo.setText(carInfoList.get(0));
                    break;
            }
        }
    };

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
                while(true){    //receive
                    try {
                        String recv = inputStream.readLine();
                        if(recv != null){
                            recv = recv.substring(1);
                            if(recv.equals("exit")){
                                client_Socket.close();
                                break;
                            }else{  //번호 4자리 + 문자 1자리를 받았어
                                if(carInfoList.size() != 0){
                                    for(int i = 0; i < 3; i++){
                                        if(recv.equals(carInfoList.get(i).substring(0,4))){
                                            registeredFlag = true;
                                        }
                                    }
                                }
                                if(registeredFlag){ //등록 돼있는게 있으면 이미 등록된 차라고 말해줘야함.
                                    tts.speak("이미 등록된 차량입니다",TextToSpeech.QUEUE_FLUSH,null);
                                }
                                else{ //등록 안됨 -> 등록시켜줘야함
                                    long now = System.currentTimeMillis();
                                    Date mDate = new Date(now);
                                    @SuppressLint("SimpleDateFormat") SimpleDateFormat simpleDate = new SimpleDateFormat("mm:ss");
                                    String getTime = simpleDate.format(mDate).substring(0,2) + simpleDate.format(mDate).substring(3);

                                    info = recv + getTime;
                                    carInfoList.add(info);

                                    //차량 등록 확인 (view 에서)
                                    Message msg = Message.obtain(null, 1, info);
                                    mainHandler.sendMessage(msg);
                                }
                            }
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        };
        checkUpdate.start();
    }

}
