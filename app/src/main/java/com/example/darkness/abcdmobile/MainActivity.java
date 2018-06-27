package com.example.darkness.abcdmobile;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Base64;
import android.util.Log;
import android.widget.Button;
import android.view.View.OnClickListener;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.zxing.integration.android.IntentIntegrator;
import com.google.zxing.integration.android.IntentResult;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.InvalidAlgorithmParameterException;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.spec.ECGenParameterSpec;
import java.security.spec.EncodedKeySpec;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.RSAPrivateKeySpec;
import java.security.spec.RSAPublicKeySpec;
import java.security.spec.X509EncodedKeySpec;


public class MainActivity extends AppCompatActivity {

    PublicKey publickey = null;
    PrivateKey privatekey = null;
    byte[] publicKeyBytes = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //ECC로 생성된 key들은 bytes -> BASE64로 인코딩된 String으로 저장된다.
        //publicKey, privateKey, wallet(sha256으로 한번 해쉬된)
        //isRetal = false, product, rentalTime, period(hour), nodeId를 따로 저장.


        Button createWalletButton = findViewById(R.id.createwalletbutton);
        createWalletButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View view) {
                //publicKey가 있는 경우도 CreateWallet부분에서 처리한다.
                Intent intent = new Intent(MainActivity.this, CreateWalletActivity.class);
                startActivity(intent);
            }
        });

        Button rentalButton = findViewById(R.id.rentalbutton);
        rentalButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                Socket socket = null;
                OutputStream stream = null;
                try {
                    socket = new Socket("192.168.1.4",7000);
                    stream = socket.getOutputStream();
                    stream.write("test".getBytes());
                } catch (IOException e) {
                    e.printStackTrace();
                    Toast.makeText(getApplicationContext(),"not connet", Toast.LENGTH_SHORT).show();
                }

                /*
                try {
                    stream = socket.getOutputStream();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                // 그리고 현재 날짜를 출력해줍니다.
                try {
                } catch (IOException e) {
                    e.printStackTrace();
                }*/
            }
        });
    }

}
