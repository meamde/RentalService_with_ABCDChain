package com.example.darkness.abcdmobile;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.google.zxing.integration.android.IntentIntegrator;
import com.google.zxing.integration.android.IntentResult;

import java.security.InvalidAlgorithmParameterException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.spec.ECGenParameterSpec;

public class CreateWalletActivity extends AppCompatActivity{

    String returnString = null;

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        IntentResult result = IntentIntegrator.parseActivityResult(requestCode, resultCode, data);
        if (requestCode == IntentIntegrator.REQUEST_CODE) {
            if(result.getContents() == null) {
                Toast.makeText(this, "Cancelled", Toast.LENGTH_LONG).show();
            } else {
                //Toast.makeText(this, "Scanned: " + result.getContents(), Toast.LENGTH_LONG).show();
                //       // result.getFormatName() : 바코드 종류
                //       // result.getContents() : 바코드 값
                returnString =  result.getContents();
                Toast.makeText(this, "Scanned: " + returnString, Toast.LENGTH_LONG).show();
            }
        } else {
            super.onActivityResult(requestCode, resultCode, data);
        }
    }
    public void startQRCode(){
        new IntentIntegrator(this).initiateScan();
    }



    public void generateKey(){

        KeyPairGenerator kpg = null;
        try {
            kpg = KeyPairGenerator.getInstance("EC");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        ECGenParameterSpec ecsp ;

        ecsp  = new ECGenParameterSpec("secp521r1");
        try {
            kpg.initialize(ecsp , new SecureRandom());
        } catch (InvalidAlgorithmParameterException e) {
            e.printStackTrace();
        }

        //랜덤으로 키 만들기.

        KeyPair kp = kpg.generateKeyPair();
        PrivateKey privateKey = kp.getPrivate();
        PublicKey publicKey = kp.getPublic();
        //여기서 노드랑 연결해서 지갑이 유효한지


        TextView txtView1 = findViewById(R.id.textView);
        txtView1.setText(publicKey.toString());
        ((TextView)findViewById(R.id.textView2)).setText(privateKey.toString());
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_createwallet);

        Button createCheckButton = findViewById(R.id.createcheckbutton);
        createCheckButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                returnString = null;
                startQRCode();
            }
        });

        Button generateButton = findViewById(R.id.generatebutton);
        generateButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(returnString == null){
                    Toast.makeText(getApplicationContext(), "First Scan the QRcode", Toast.LENGTH_SHORT).show();
                    return ;
                }
                else{
                    //키 생성과

                    //노드와 소켓 연결해서 지갑이 유효한지 체크.
                }
            }
        });
    }
}
