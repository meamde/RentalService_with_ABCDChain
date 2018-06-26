package com.example.darkness.abcdmobile;

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

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;

import java.nio.charset.StandardCharsets;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
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

        KeyPairGenerator keyGenerator = null;
        try {
            keyGenerator = KeyPairGenerator.getInstance("RSA");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        keyGenerator.initialize(1024);
        KeyPair keyPair = keyGenerator.generateKeyPair();
        publickey = keyPair.getPublic();
        privatekey = keyPair.getPrivate();
        Button genButton = findViewById(R.id.GenButton);
        genButton.setOnClickListener(new OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.KITKAT)
            @Override
            public void onClick(View view) {
                SecureRandom random = new SecureRandom();
                try {
                    SharedPreferences prefs = getSharedPreferences("PREF", MODE_PRIVATE);
                    boolean isGenerated = prefs.getBoolean("isGenerated",false);
                    if(isGenerated){
                        Toast.makeText(getApplicationContext(),"Already you have key", Toast.LENGTH_LONG).show();
                        return ;
                    }
                    publicKeyBytes = publickey.getEncoded();
                    KeyFactory keyFactory = KeyFactory.getInstance("RSA");
                    EncodedKeySpec publicKeySpec = new X509EncodedKeySpec(publicKeyBytes);

                    RSAPublicKeySpec publicSpec = keyFactory.getKeySpec(publickey, RSAPublicKeySpec.class);
                    RSAPrivateKeySpec privateSpec = keyFactory.getKeySpec(privatekey, RSAPrivateKeySpec.class);
                    String publicKeyModulus = publicSpec.getModulus().toString(16);
                    String publicKeyExponent = publicSpec.getPublicExponent().toString(16);
                    SharedPreferences.Editor ed = prefs.edit();
                    ed.putBoolean("isGenerated", true);
                    ed.putString("publicKeyStr",Base64.encodeToString(publickey.getEncoded(), Base64.DEFAULT));
                    ed.putString("privateKeyStr",Base64.encodeToString(privatekey.getEncoded(), Base64.DEFAULT));
                    ed.commit();
                    EditText pubText = findViewById(R.id.PublicKey);
                    pubText.setText(publicSpec.toString());
                    EditText priText = findViewById(R.id.PirvateKey);
                    priText.setText(publickey.toString());


                } catch (NoSuchAlgorithmException e) {
                    e.printStackTrace();
                } catch (InvalidKeySpecException e) {
                    e.printStackTrace();
                }

            }
        });


        Button btn = findViewById(R.id.button);
        btn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {

                SharedPreferences prefs = getSharedPreferences("PREF", MODE_PRIVATE);
                String publicStr =  prefs.getString("publicKeyStr",null);

                if(publicStr == null){
                    Toast.makeText(getApplicationContext(), "null str", Toast.LENGTH_LONG).show();
                    return ;
                }
                byte[] publicKeyBytes = Base64.decode(publicStr, Base64.DEFAULT);
                X509EncodedKeySpec spec = new X509EncodedKeySpec(publicKeyBytes);
                PublicKey key = null;
                try {
                    key = KeyFactory.getInstance("RSA").generatePublic(spec);
                } catch (InvalidKeySpecException e) {
                    e.printStackTrace();
                } catch (NoSuchAlgorithmException e) {
                    e.printStackTrace();
                }

                Toast.makeText(getApplicationContext(),key.toString(), Toast.LENGTH_LONG).show();

         }
        });
    }
}
