package com.example.darkness.abcdmobile;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Base64;
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
                returnString =  result.getContents();
            }
        } else {
            super.onActivityResult(requestCode, resultCode, data);
        }
    }
    public void startQRCode(){
        new IntentIntegrator(this).initiateScan();
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
                    //Zxing IntentIntegrator부분을 동기식으로 구현을 못해서 이렇게 함.
                    Toast.makeText(getApplicationContext(), "First Scan the QRcode", Toast.LENGTH_SHORT).show();
                    return ;
                }
                else{
                    //키 생성과
                    KeyPairGenerator kpg = null;
                    try {
                        kpg = KeyPairGenerator.getInstance("EC");
                    } catch (NoSuchAlgorithmException e) {
                        e.printStackTrace();
                    }
                    ECGenParameterSpec ecsp ;

                    ecsp  = new ECGenParameterSpec("secp521r1");
                    try {
                        //랜덤으로 키 만들기.
                        kpg.initialize(ecsp , new SecureRandom());
                    } catch (InvalidAlgorithmParameterException e) {
                        e.printStackTrace();
                    }

                    KeyPair kp = kpg.generateKeyPair();
                    PrivateKey privateKey = kp.getPrivate();
                    PublicKey publicKey = kp.getPublic();

                    //노드와 소켓 연결해서 지갑이 유효한지 체크.
                    //Socket부분 연동시켜서 publicKey, privateKey부분 넘기기.

                    SharedPreferences prefs = getSharedPreferences("PREF", MODE_PRIVATE);
                    Boolean isGen = prefs.getBoolean("isGenerated", false);

                    Toast.makeText(getApplicationContext(),isGen.toString(), Toast.LENGTH_SHORT).show();

                    TextView publicKeyView = findViewById(R.id.publickeyview);

                    publicKeyView.setText(publicKey.toString());

                    //지갑키가 유효하다면.
                    /*
                    SharedPreferences prefs = getSharedPreferences("PREF", MODE_PRIVATE);
                    SharedPreferences.Editor ed = prefs.edit();
                    ed.putBoolean("isGenerated", true);
                    //publicKey, privateKey는 바이트를 구해서 그걸 Base64를 이용 문자로 인코딩을 한다.
                    ed.putString("publicKeyStr", Base64.encodeToString(publicKey.getEncoded(), Base64.DEFAULT));
                    ed.putString("privateKeyStr",Base64.encodeToString(privateKey.getEncoded(), Base64.DEFAULT));
                    ed.commit();
                    */
                }
            }
        });
    }
}
