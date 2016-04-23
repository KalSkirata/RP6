package com.example.ziri.rp6_wifi;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;


public class Client extends Activity {

    private Socket socket;
    InputStream is;
    boolean etat_pause = true;
    PrintWriter out;
    private ProgressBar batterie;
    private BufferedReader input;
    public static final int BUFFER_SIZE = 2048;
    private BufferedReader in = null;


    private static final int SERVERPORT = 2001;
    private static final String SERVER_IP = "192.168.1.18";

    public static final String TAG = "MyActivity";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button conn = (Button) findViewById(R.id.connexion);
        Button deconn = (Button) findViewById(R.id.deconnexion);
        Button up = (Button) findViewById(R.id.up);
        Button right = (Button) findViewById(R.id.right);
        Button down = (Button) findViewById(R.id.down);
        Button left = (Button) findViewById(R.id.left);
        batterie = (ProgressBar) findViewById(R.id.batterie);
        batterie.setProgress(0);


    }

    public void onClick(View view) {


/*            try {

                String in = input.readLine();
                Log.e(TAG, in);
                /*if (isInteger(in)) {
                    Log.e(TAG, in);
                    batterie.setProgress(Integer.parseInt(in));
                }


            } catch (IOException e) {
            }catch (NullPointerException e){

            }*/

        final int id = view.getId();

        switch (id) {

            case R.id.connexion:
                try {
                    new Thread(new ClientThread()).start();

                    this.input = new BufferedReader(new InputStreamReader(this.socket.getInputStream()));

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
                    //out.flush();
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
                    //out.close();

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
                    //out.flush();
                    Log.e(TAG, "tourner a gauche");
                    //out.close();
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
                    //out.flush();
                    //out.close();
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
                    //out.close();
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

                    //out.close();
                    Toast.makeText(getApplicationContext(),
                            " le robot est en marche", Toast.LENGTH_LONG).show();


                    etat_pause = true;
                }
                }catch (Exception e){
                    e.printStackTrace();}
                break;

            case R.id.batterie:
                try {
                    byte[] buffer = new byte[1024];
                    int bytesRead;
                    String str = "Battery";
                    PrintWriter out = new PrintWriter(new BufferedWriter(
                            new OutputStreamWriter(socket.getOutputStream())),
                            true);
                    out.println(str);
                    //out.flush();
                    //out.close();
                    //is = socket.getInputStream();

                    Toast.makeText(Client.this, receiveDataFromServer(), Toast.LENGTH_SHORT).show();

                    //String in = input.readLine();
                    //Log.e(TAG, in);

                /*if (isInteger(in)) {
                    Log.e(TAG, in);
                    batterie.setProgress(Integer.parseInt(in));
                }*/
                    Log.e(TAG, " etat de la batterie");
                    Toast.makeText(getApplicationContext(),
                            "batterie", Toast.LENGTH_LONG).show();
                }catch (Exception e){
                    e.printStackTrace();}
                break;
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

    public String receiveDataFromServer() {
        try {
            String message = "";
            int charsRead = 0;
            char[] buffer = new char[BUFFER_SIZE];

            while ((charsRead = in.read(buffer)) != -1) {
                message += new String(buffer).substring(0, charsRead);
            }

            //disConnectWithServer(); // disconnect server
            return message;
        } catch (IOException e) {
            return "Error receiving response:  " + e.getMessage();
        }
    }



}