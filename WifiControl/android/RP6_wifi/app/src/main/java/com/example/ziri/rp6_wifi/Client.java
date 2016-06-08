package com.example.ziri.rp6_wifi;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.Toast;
import android.widget.ToggleButton;


public class Client extends Activity {

    private Socket socket;
    boolean etat_pause = true;
    PrintWriter out;
    private ProgressBar batterie;
    ToggleButton bumperright;
    ToggleButton bumperleft;
    EditText lightr;
    EditText lightl;

    //Handler mHandler;


    private static final int SERVERPORT = 2001;
    private static final String SERVER_IP = "192.168.137.10";

    public static final String TAG = "MyActivity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        setContentView(R.layout.activity_main);
        Button conn = (Button) findViewById(R.id.connexion);
        Button deconn = (Button) findViewById(R.id.deconnexion);
        Button up = (Button) findViewById(R.id.up);
        Button right = (Button) findViewById(R.id.right);
        Button down = (Button) findViewById(R.id.down);
        Button left = (Button) findViewById(R.id.left);
        batterie = (ProgressBar) findViewById(R.id.batterie);
        batterie.setProgress(0);
        batterie.setMax(1000);

        bumperleft = (ToggleButton) findViewById(R.id.bumpleft);
        bumperright = (ToggleButton) findViewById(R.id.bumpright);
        bumperright.setEnabled(true);
        bumperleft.setEnabled(true);

        lightl = (EditText) findViewById(R.id.lightleft);
        lightr = (EditText) findViewById(R.id.lightright);
        lightl.setKeyListener(null);
        lightr.setKeyListener(null);



        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        //  this.mHandler = new Handler();
        // m_Runnable.run();


    }

/*
    private final Runnable m_Runnable = new Runnable()
    {
        public void run()

        {
            Toast.makeText(Client.this,"in runnable",Toast.LENGTH_SHORT).show();

            Client.this.mHandler.postDelayed(m_Runnable,10000);
        }

    };
*/

    public void onClick(View view) {


        final int id = view.getId();

        switch (id) {

            case R.id.connexion:
                try {
                    new Thread(new ClientThread()).start();

                    //this.input = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));


                    /*String str = "connection";
                    PrintWriter out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(str);

                    Log.e(TAG, "je suis dans la connexion");
                    //out.close();
                    Toast.makeText(getApplicationContext(),
                            "connection established", Toast.LENGTH_LONG).show();
*/
                } catch (Exception e) {
                    e.printStackTrace();}
                break;

            case R.id.up:
                try {

                    String fwd = "fwd";
                    PrintWriter out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(fwd);
                    sensors();

                    //out.flush();
                    Log.e(TAG, " avancer vers l'avant");

                    Toast.makeText(getApplicationContext(),
                            "go forward", Toast.LENGTH_LONG).show();
                    //out.close();
                }catch (Exception e){
                    e.printStackTrace();}

                break;

            case R.id.right:
                try {
                    String rgt = "right";
                    PrintWriter out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(rgt);
                    sensors();
                    Log.e(TAG, " tourner a droite");
                    //out.close();
                    Toast.makeText(getApplicationContext(),
                            "turn right", Toast.LENGTH_LONG).show();

                }catch (Exception e){
                    e.printStackTrace();}
                break;

            case R.id.down:
                try {

                    String back = "bwd";
                    out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(back);
                    out.flush();
                    Log.e(TAG, " reculer");
                    sensors();

                    Toast.makeText(getApplicationContext(),
                            "get back", Toast.LENGTH_LONG).show();

                }catch (Exception e){

                    e.printStackTrace();}
                break;

            case R.id.left:
                try {
                    String lft = "left";
                    PrintWriter out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(lft);
                    sensors();
                    Log.e(TAG, "tourner a gauche");

                    Toast.makeText(getApplicationContext(),
                            "turn left", Toast.LENGTH_LONG).show();

                }catch (Exception e){
                    e.printStackTrace();}
                break;

            case R.id.deconnexion:
                try {

                    String str = "disconnection";
                    PrintWriter out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(str);

                    socket.close();
                    Log.e(TAG, " la socket est fermé");
                    Toast.makeText(getApplicationContext(),
                            "disconnection", Toast.LENGTH_LONG).show();
                }catch (Exception e){
                    e.printStackTrace();}
                break;

            case R.id.stop:
                try {

                    if (etat_pause == true) {
                        String stp = "stop";
                        PrintWriter out = new PrintWriter(new BufferedWriter(
                                new OutputStreamWriter(socket.getOutputStream())),
                                true);
                        out.println(stp);
                        out.flush();
                        Log.e(TAG, " arreter ");

                        Toast.makeText(getApplicationContext(),
                                "le robot est arreter", Toast.LENGTH_LONG).show();

                        etat_pause = false;

                    }
                    else {

                        String srt = "start";
                        PrintWriter out = new PrintWriter(new BufferedWriter(
                                new OutputStreamWriter(socket.getOutputStream())),
                                true);
                        out.println(srt);
                        out.flush();
                        sensors();
                        Log.e(TAG, " start ");

                        Toast.makeText(getApplicationContext(),
                                " le robot est en marche", Toast.LENGTH_LONG).show();


                        etat_pause = true;
                    }
                }catch (Exception e){
                    e.printStackTrace();}
                break;

            /*case R.id.batterie:
                try {


                    String str = "Battery";
                    PrintWriter out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(str);

                    BufferedReader in = new BufferedReader(
                            new InputStreamReader( socket.getInputStream()));

                    //int inputLine = Integer.parseInt(in.readLine());
                    String msg = in.readLine();

                    String data [] = msg.split("/");

                    if (data[0].equals("bat")){
                        batterie.setProgress(Integer.parseInt(data[1]));
                    }

                    Log.e(TAG, data[0]);
                    Log.e(TAG, data[1]);

                    String val = String.valueOf(in.readLine());

                    if (val.length() >= 3) {
                        //String aString = Integer.toString(inputLine);


                        Toast.makeText(getApplicationContext(),
                                val, Toast.LENGTH_LONG).show();
                    }
                    Log.e(TAG, val);


                    Log.e(TAG, "batterie");
                    Toast.makeText(getApplicationContext(),
                            "batterie", Toast.LENGTH_LONG).show();
                }catch (Exception e){
                    e.printStackTrace();
                }
            break;*/
        }
    }

    class ClientThread implements Runnable {

        @Override
        public void run() {

            try {
                InetAddress serverAddr = InetAddress.getByName(SERVER_IP);

                socket = new Socket(serverAddr, SERVERPORT);

            } catch (UnknownHostException e1) {
                e1.printStackTrace();
            } catch (IOException e1) {
                e1.printStackTrace();
            }


        }

    }

    public boolean isInteger( String str){
        try{
            Integer.parseInt(str);
        }catch(NumberFormatException e){
            return false;
        }catch (NullPointerException e){
            return false;
        }
        return true;
    }

    public void sensors() {
        try {

            BufferedReader in = new BufferedReader(
                    new InputStreamReader(socket.getInputStream()));


            String msg = in.readLine();
            //Log.e(TAG,msg);
            String data[] = msg.split(",");
            String result[][] = new String[3][3];

            for (int i = 0; i < data.length; i++)
            {
                result[i] = data[i].split("/");

            }
            if (result[0][0].equals("bat")) {
                batterie.setProgress(Integer.parseInt(result[0][1]));
                Log.e(TAG,"[0][1]"+result[0][1]);
            }

            if (result[1][0].equals("light")) {
                lightl.setText(Integer.parseInt(result[1][1])/10+"%");
                lightr.setText(Integer.parseInt(result[1][2])/10+"%");
                Log.e(TAG,"[1][1]"+result[1][1]);
                Log.e(TAG,"[1][2]"+result[1][2]);
            }

            if (result[2][0].equals("acs")) {
                bumperleft.setChecked(Boolean.parseBoolean(result[2][1]));
                bumperright.setChecked(Boolean.parseBoolean(result[2][2]));
                Log.e(TAG,"[2][1]"+result[2][1]);
                Log.e(TAG,"[2][2]"+result[2][2]);
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}