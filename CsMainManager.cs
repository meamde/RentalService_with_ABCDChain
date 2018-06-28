using Org.BouncyCastle.Asn1.Sec;
using Org.BouncyCastle.Asn1.X9;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Generators;
using Org.BouncyCastle.Crypto.Parameters;
using Org.BouncyCastle.Crypto.Signers;
using Org.BouncyCastle.Math;
using Org.BouncyCastle.Math.EC;
using Org.BouncyCastle.Security;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using ZXing;
using ZXing.QrCode;
using LitJson;
using System.IO;

public class CsMainManager : MonoBehaviour
{
    //배경 GameObject
    public GameObject initObject;
    public GameObject mainObject;
    public GameObject rentalObject;
    public GameObject stateObject;
    public GameObject returnObject;

    //특수 GameObject
    public GameObject curGameObject;
    public GameObject webCamGameObject;

    // Use this for initializationprivate
    public Text stateText;
    public Text myStateText;
    public Text returnText;
    private bool isQRMode = false;
    public WebCamTexture camTexture;
    public RawImage rawImage;
    public Dropdown periodDropDown;
    private string nodeIP;
    private int nodePort;
    IBarcodeReader barcodeReader;
    public bool isInquire;
    byte[] buffer;

    //개인키와 사설키.
    string publicKey;   //두개 전부 2문자가 1byte가 되는 HexString으로 인코딩된 형태.
    string privateKey;
    string userAddress;
    
    public string bytesToHexString(byte[] bytes)
    {
        string hex = BitConverter.ToString(bytes); // AB-CD-EF-01
        return hex.Replace("-", ""); // ABCDEF01
    }

    public byte[] HexStringTobytes(String hexStr)
    {
        byte[] bytes = new byte[hexStr.Length / 2];
        for (int i = 0; i < hexStr.Length / 2; i++) bytes[i] = Convert.ToByte(hexStr.Substring(i * 2, 2), 16);
        return bytes;
    }

    public bool isInit()
    {
        //Debug.Log(PlayerPrefs.GetString("PublicKey", null));
        publicKey = PlayerPrefs.GetString("PublicKey", null);
        privateKey = PlayerPrefs.GetString("PrivateKey", null);
        userAddress = PlayerPrefs.GetString("UserAddress", null);
        if(publicKey.Length == 0)
            //개인키의 길이가 0일경우
        {
            curGameObject = initObject;
            return true;
        }
        curGameObject = mainObject;
        return false;
    }

    void Start()
    {
        camTexture = new WebCamTexture();
        rawImage.texture = camTexture;
        webCamGameObject.SetActive(false);

        isInquire = false;
        publicKey = null;
        privateKey = null;
        userAddress = null;
        nodeIP = null;
        nodePort = 0;

        buffer = new byte[2048];


        //PlayerPrefs.SetString("PublicKey", null);
        //PlayerPrefs.SetString("PrivateKey", null);
        //PlayerPrefs.SetString("UserAddress", null);
        
        if (isInit())
        {
            //맨 처음이면 Init gmaeobject만 Active하게.
            initObject.SetActive(true);
            mainObject.SetActive(false);
            curGameObject = initObject;
        }
        else
        {
            initObject.SetActive(false);
            mainObject.SetActive(true);
            curGameObject = mainObject;
            //메인 화면을 띄운다.
        }

        
        
    }

	// Update is called once per frame
	void Update () {

        if (isQRMode)
        {
            //카메라 이용해서 update시 마다 QR코드 찾는 것 찾으면 바로 종료됨.
            try
            {
                IBarcodeReader barcodeReader = new BarcodeReader();
                // decode the current frame
                var result = barcodeReader.Decode(camTexture.GetPixels32(), camTexture.width, camTexture.height);
                if (result != null)
                {
                    //Debug.Log("DECODED TEXT FROM QR:" + result.Text);
                    camTexture.Pause();
                    isQRMode = false;
                    webCamGameObject.SetActive(false);

                    string[] part = result.Text.Split('/');
                    nodeIP = part[0];
                    nodePort = Int32.Parse(part[1]);
                    //찾으면 nodeIP란 string에 저장.
                }
            }
            catch (System.Exception ex) { Debug.LogWarning(ex.Message); }

        }
    }
    
    public void changeState(){
        //배경이 변할때마다 변수를 null로 해야하는 것들.
        nodeIP = null;
    }

    public void OnClickQRreadButton()
    {
        //update부분에서 계속 QR코드가 있는지 찾는다.
        if (camTexture != null)
        {
            nodeIP = null;
            webCamGameObject.SetActive(true);
            isQRMode = true;
            camTexture.Play();
        }
    }
    
    public void OnClickCreateWalletButton()
    {
        if(nodeIP == null)
        {
            return;
        }
        // packet.GetString();
        bool isVerify = false;
        while (!isVerify)
        {
            string sendJson = null;
            string sendJsonHeader = null;
            string sendJsonBody = null;

            JsonData packet = new JsonData();
            JsonData header = new JsonData();
            JsonData body = new JsonData();
            
            packet["Header"] = header;
            packet["Body"] = body;

            //"secp256k1" 세부설정.
            X9ECParameters ec = SecNamedCurves.GetByName("secp256k1");
            ECDomainParameters domainParams = new ECDomainParameters(ec.Curve, ec.G, ec.N, ec.H);
            SecureRandom random = new SecureRandom();

            // Generate EC KeyPair
            ECKeyPairGenerator keyGen = new ECKeyPairGenerator();
            ECKeyGenerationParameters keyParams = new ECKeyGenerationParameters(domainParams, random);
            keyGen.Init(keyParams);
            AsymmetricCipherKeyPair keyPair = keyGen.GenerateKeyPair();
            ECPublicKeyParameters publicKeyPara = keyPair.Public as ECPublicKeyParameters;
            ECPrivateKeyParameters privateKeyPara = keyPair.Private as ECPrivateKeyParameters;
            

            ECPoint q = publicKeyPara.Q;
            FpPoint fp = new FpPoint(ec.Curve, q.AffineXCoord, q.AffineYCoord);
            byte[] enc = fp.GetEncoded(true);
            string compressedPubKey = BitConverter.ToString(enc).Replace("-", "");
            //HexStr으로 표현한 값. 이 publicKey로도 서명이 증명가능.


            SHA256 mySHA256 = SHA256Managed.Create();
            byte[] hashPublicKey = mySHA256.ComputeHash(HexStringTobytes(compressedPubKey));
            string hexHashPublicKey = bytesToHexString(hashPublicKey);
            sendJsonBody = " \"Body\" : { \"Transaction\" : { \"Type\" : 0, \"UserAddress\" : \""
            + hexHashPublicKey + "\" } }";
            JsonData transaction = new JsonData();
            body["Transaction"] = transaction;
            transaction["Type"] = 0;
            transaction["UserAddress"] = hexHashPublicKey;


            //publicKey를 헥사String으로 변환 후 바이트형식으로 인코딩한걸 해쉬
            ECDsaSigner privSigner = new ECDsaSigner();
            privSigner.Init(true, privateKeyPara);
            BigInteger[] signature = privSigner.GenerateSignature(Encoding.ASCII.GetBytes(packet["Body"].ToJson()));
            BigInteger r = signature[0];
            BigInteger s = signature[1];
            sendJsonHeader = " \"Header\" : { \"Type\" : \"WalletRegist\", \"PublicKey\" : \"" + compressedPubKey +
                "\", \"EncryptR\" : \"" + bytesToHexString(r.ToByteArray()) + "\", \"EncryptS\" : \"" +
                bytesToHexString(s.ToByteArray()) + "\" }";
            header["Type"] = "WalletRegist";
            header["PublicKey"] = compressedPubKey;
            header["EncryptR"] = bytesToHexString(r.ToByteArray());
            header["EncryptS"] = bytesToHexString(s.ToByteArray());

            sendJson = "{ " + sendJsonHeader + ", " + sendJsonBody + " }";

            byte[] walletRegistOutput = Encoding.ASCII.GetBytes(packet.ToJson());
            //Json형식  Str를 byte Array로 해서 보낸다.

            /*
            // 대칭키 인증서 인증이 맞는지 테스트.
            BigInteger biPublicKey = new BigInteger(HexStringTobytes(compressedPubKey));
            ECPoint q2 = domainParams.Curve.DecodePoint(biPublicKey.ToByteArray());
            publicKeyPara = new ECPublicKeyParameters(q2, domainParams);

            ECDsaSigner pubSigner = new ECDsaSigner();
            pubSigner.Init(false, publicKeyPara);
            Debug.Log(pubSigner.VerifySignature(Encoding.ASCII.GetBytes(packet["Body"].ToJson()), r, s));
            */

            Debug.Log(packet.ToJson());

            TcpClient client = new TcpClient(nodeIP, nodePort);
            //TcpClient client = new TcpClient("13.209.78.154", 51166);
            NetworkStream stream = client.GetStream();
            stream.Write(walletRegistOutput, 0, walletRegistOutput.Length);
            System.Array.Clear(buffer, 0, buffer.Length);
            stream.Read(buffer, 0, buffer.Length);

            JsonData registResponse = JsonMapper.ToObject(Encoding.ASCII.GetString(buffer));
            Debug.Log(Encoding.ASCII.GetString(buffer));
            if (registResponse["Body"]["Response"].GetNatural() == 1)
            {

                // 만약 등록되면
                PlayerPrefs.SetString("PublicKey", compressedPubKey);
                PlayerPrefs.SetString("PrivateKey", bytesToHexString(privateKeyPara.D.ToByteArray()));
                PlayerPrefs.SetString("UserAddress", hexHashPublicKey);
                
                //지갑이 유효하면 while문을 벗어난다.
                isVerify = true;
            }
            client.Close();
            //우선 소켓을 닫는다.
        }
        changeState();
        OnClickHomeButton();
    }

    public void OnClickRentalButton()
    {
        curGameObject = rentalObject;
        mainObject.SetActive(false);
        rentalObject.SetActive(true);
        changeState();
        stateText.text = "First, Read the QRCode!";

    }

    public void OnClickStateButton()
    {
        curGameObject = stateObject;
        mainObject.SetActive(false);
        stateObject.SetActive(true);
        changeState();

        myStateText.text = "publicKey : " + publicKey + "\n" + "walletKey : " + userAddress;
    }

    public void OnClickReturnButton()
    {
        curGameObject = returnObject;
        mainObject.SetActive(false);
        returnObject.SetActive(true);
        changeState();
        returnText.text = "First, Read the QRCode!";
        isInquire = false;
    }

    public void testClick()
    {

        JsonData initialConnectPacket = new JsonData();


        JsonData body = new JsonData(); JsonData header = new JsonData();
        initialConnectPacket["Body"] = body;
        initialConnectPacket["Header"] = header;
        header["Type"] = "InitialConnect";
        header["PublicKey"] = publicKey;


        X9ECParameters ec = SecNamedCurves.GetByName("secp256k1");
        ECDomainParameters domainParams = new ECDomainParameters(ec.Curve, ec.G, ec.N, ec.H);
        BigInteger biPrivateKey = new BigInteger(HexStringTobytes(privateKey));
        ECPrivateKeyParameters tmpPrivatePara = new ECPrivateKeyParameters(biPrivateKey, domainParams);
        //pivateKey로 privatekeyPara 만드는부분

        ECDsaSigner privSigner = new ECDsaSigner();
        privSigner.Init(true, tmpPrivatePara);
        BigInteger[] signature = privSigner.GenerateSignature(Encoding.ASCII.GetBytes(initialConnectPacket["Body"].ToJson()));
        BigInteger r = signature[0];
        BigInteger s = signature[1];
        //디지털 서명을 하는 부분.

        header["EncryptR"] = bytesToHexString(r.ToByteArray());
        header["EncryptS"] = bytesToHexString(s.ToByteArray());
        body["UserAddress"] = userAddress;
        //initial Connect하는 부분 Json완성.



        //Debug.Log(initialConnectPacket.ToJson());

        byte[] initialConnectPacketOutPut = Encoding.ASCII.GetBytes(initialConnectPacket.ToJson());
        //TcpClient client = new TcpClient(nodeIP, nodePort);
        TcpClient client = new TcpClient("13.125.135.232", 58243);
        NetworkStream stream = client.GetStream();
        stream.Write(initialConnectPacketOutPut, 0, initialConnectPacketOutPut.Length);
        //Debug.Log(buffer.Length);
        byte[] tbuffer = new byte[4096];
        System.Array.Clear(tbuffer, 0, tbuffer.Length);

        stream.Read(tbuffer, 0, 4096);

        Debug.Log(Encoding.ASCII.GetString( tbuffer));

        client.Close();
    }

    public void OnClickHomeButton()
    {
        curGameObject.SetActive(false);
        curGameObject = mainObject;
        mainObject.SetActive(true);
        changeState();
        stateText.text = null;
    }

    public void OnClickConfirmButton()
    {
        /*
        if(nodeIP == null)
        {
            Debug.Log("NULL IP");
            stateText.text = "First, Read QRCode";
            return;
        }
        */
        JsonData initialConnectPacket = new JsonData();


        JsonData body = new JsonData(); JsonData header = new JsonData();
        initialConnectPacket["Body"] = body;
        initialConnectPacket["Header"] = header;
        header["Type"] = "InitialConnect";
        header["PublicKey"] = publicKey;


        X9ECParameters ec = SecNamedCurves.GetByName("secp256k1");
        ECDomainParameters domainParams = new ECDomainParameters(ec.Curve, ec.G, ec.N, ec.H);
        BigInteger biPrivateKey = new BigInteger(HexStringTobytes(privateKey));
        ECPrivateKeyParameters tmpPrivatePara = new ECPrivateKeyParameters(biPrivateKey, domainParams);
        //pivateKey로 privatekeyPara 만드는부분

        ECDsaSigner privSigner = new ECDsaSigner();
        privSigner.Init(true, tmpPrivatePara);
        BigInteger[] signature = privSigner.GenerateSignature(Encoding.ASCII.GetBytes(initialConnectPacket["Body"].ToJson()));
        BigInteger r = signature[0];
        BigInteger s = signature[1];
        //디지털 서명을 하는 부분.

        header["EncryptR"] = bytesToHexString(r.ToByteArray());
        header["EncryptS"] = bytesToHexString(s.ToByteArray());
        body["UserAddress"] = userAddress;
        //initial Connect하는 부분 Json완성.

        Debug.Log("test");
        byte[] initialConnectPacketOutPut = Encoding.ASCII.GetBytes(initialConnectPacket.ToJson());

        //TcpClient client = new TcpClient(nodeIP, nodePort);
        TcpClient client = new TcpClient("13.209.78.154", 51166);
        NetworkStream stream = client.GetStream();
        stream.Write(initialConnectPacketOutPut, 0, initialConnectPacketOutPut.Length);
        //initConnect packet 보냄.

        System.Array.Clear(buffer, 0, buffer.Length);
        stream.Read(buffer, 0, buffer.Length);
        JsonData initialConnectResponsePacket = JsonMapper.ToObject(Encoding.ASCII.GetString(buffer));
        Debug.Log(Encoding.ASCII.GetString(buffer));
        string deviceId = initialConnectResponsePacket["Body"]["DeviceId"].GetString();
        if (initialConnectResponsePacket["Body"]["Available"].GetNatural() == 0)
        {
            //빌릴 수 없는 Device
            client.Close();
            stateText.text = "Can't Rental " + deviceId;
            return;
        }
        int devicePricePerHour = (int)initialConnectResponsePacket["Body"]["DevicePrice"].GetNatural();
        int Amount = (int)initialConnectResponsePacket["Body"]["Amount"].GetNatural();
        if(devicePricePerHour > Amount)
        {
            //돈이 없어서 못빌림.
            client.Close();
            stateText.text = "Can't Rental " + deviceId + ", you don't have money. ";
            return;
        }
        int hour = Int32.Parse(periodDropDown.options[periodDropDown.value].text);
        //지금 컨펌하는, 빌릴시간 구하는 부분.

        if(hour * devicePricePerHour > Amount)
        {
            //이것도 돈이 없어서 못빌림.
            client.Close();
            stateText.text = "Can't Rental " + deviceId + ", Lower the period. ";
            return;
        }


        //1970~현재 몇초인지.
        DateTime epoch = new DateTime(1970, 1, 1, 0, 0, 0);//, DateTimeKind.Utc);
        long tSecond = (long)(new DateTime(DateTime.Now.Ticks) - epoch).TotalSeconds; //c의 time_t()와 같게 할려고.
        Debug.Log(tSecond);
        

        JsonData rentalRequestPacket = new JsonData();
        header = new JsonData(); body = new JsonData();
        rentalRequestPacket["Header"] = header;
        rentalRequestPacket["Body"] = body;
        JsonData transaction = new JsonData();
        body["Transaction"] = transaction;
        transaction["Amount"] = devicePricePerHour * hour;
        transaction["RentalDevice"] = deviceId;
        transaction["RentalTime"] = tSecond;
        transaction["Type"] = 3;
        transaction["UserAddress"] = userAddress;

        header["PublicKey"] = publicKey;
        header["Type"] = "RentalRequest";

        privSigner = new ECDsaSigner();
        privSigner.Init(true, tmpPrivatePara);
        signature = privSigner.GenerateSignature(Encoding.ASCII.GetBytes(initialConnectPacket["Body"].ToJson()));
        r = signature[0];
        s = signature[1];
        //디지털 서명을 하는 부분.
        header["EncryptR"] = bytesToHexString(r.ToByteArray());
        header["EncryptS"] = bytesToHexString(s.ToByteArray());

        byte[] rentalRequestPacketOutput = Encoding.ASCII.GetBytes( rentalRequestPacket.ToJson());

        stream.Write(rentalRequestPacketOutput, 0, rentalRequestPacketOutput.Length);
        System.Array.Clear(buffer, 0, buffer.Length);
        stream.Read(buffer, 0, buffer.Length);

        JsonData rentalResponsePacket = JsonMapper.ToObject(Encoding.ASCII.GetString(buffer));
        if(rentalResponsePacket["Body"]["Response"].GetNatural() == 1)
        {
            stateText.text = "YOU USE RENTALSERVICE!";
        }
        else
        {
            stateText.text = "Something wrong happend";
        }
        client.Close();
        return;
    }

    public void OnClickShowState()
    {
        if(nodeIP == null)
        {
            myStateText.text = "First, Click QRCode Button!";
            return;
        }
        //여러 정보들을 조회하고 받은 값들을 적절한 String으로 변환한다.

    }

    public void OnClickInquireButton() //QueryButton?
    {
        //반납하기 전 조회하는 함수.
        if (nodeIP == null)
        {
            returnText.text = "First, Click QRCode Button!";
            return;
        }
        //여기서 잘 이뤄지면
        JsonData getAddressAmountPacket = new JsonData();
        JsonData body = new JsonData(); JsonData header = new JsonData();
        getAddressAmountPacket["Body"] = body;
        getAddressAmountPacket["Header"] = header;
        body["UserAddress"] = userAddress;


        X9ECParameters ec = SecNamedCurves.GetByName("secp256k1");
        ECDomainParameters domainParams = new ECDomainParameters(ec.Curve, ec.G, ec.N, ec.H);
        BigInteger biPrivateKey = new BigInteger(privateKey);
        ECPrivateKeyParameters tmpPrivatePara = new ECPrivateKeyParameters(biPrivateKey, domainParams);
        

        ECDsaSigner privSigner = new ECDsaSigner();
        privSigner.Init(true, tmpPrivatePara);
        BigInteger[] signature = privSigner.GenerateSignature(Encoding.ASCII.GetBytes(getAddressAmountPacket["Body"].ToJson()));
        BigInteger r = signature[0];
        BigInteger s = signature[1];
        //디지털 서명을 하는 부분.
        header["EncryptR"] = bytesToHexString(r.ToByteArray());
        header["EncryptS"] = bytesToHexString(s.ToByteArray());
        header["PublicKey"] = publicKey;
        header["Type"] = "GetAddressAmount";
        //returnText.text = "deviceId"
        //여기에 저장합

        byte[] getAddressAmountPacketOutput = Encoding.ASCII.GetBytes(getAddressAmountPacket.ToJson());
        TcpClient client = new TcpClient(nodeIP, nodePort);
        NetworkStream stream = client.GetStream();
        stream.Write(getAddressAmountPacketOutput, 0, getAddressAmountPacketOutput.Length);
        System.Array.Clear(buffer, 0, buffer.Length);
        stream.Read(buffer, 0, buffer.Length);
        JsonData getAddressAmountResponsePacket = JsonMapper.ToObject(Encoding.ASCII.GetString(buffer));
        client.Close();
        if (getAddressAmountResponsePacket["Body"]["DeviceId"].Equals(null))
        {
            //null값이니 빌린 물건이 없다는 뜻
            isInquire = false;
            returnText.text = "no rental device";
            return;
        }
        returnText.text = getAddressAmountResponsePacket["Body"]["DeviceId"].ToJson();
        isInquire = true;
    }


    public void OnClicktDetermineButton()
    {
        //물건을 return확정하는 버튼.

        if (nodeIP == null)
        {
            returnText.text = "First, Click QRCode Button!";
            return;
        }
        else if(!isInquire)
        {
            returnText.text = "Second, No return Device!!";
            return;
        }
        string deviceId = returnText.text;

        //1970~현재 몇초인지.
        DateTime epoch = new DateTime(1970, 1, 1, 0, 0, 0);//, DateTimeKind.Utc);
        long tSecond = (long)(new DateTime(DateTime.Now.Ticks) - epoch).TotalSeconds; //c의 time_t()와 같게 할려고.
        Debug.Log(tSecond);

        //빌린 물건과 현제 시간만 보낸다.

        JsonData returnRequestPacket = new JsonData();
        JsonData header = new JsonData();  JsonData body = new JsonData();
        returnRequestPacket["Header"] = header;
        returnRequestPacket["Body"] = body;
        JsonData transaction = new JsonData();
        returnRequestPacket["Body"]["Transaction"] = transaction;
        transaction["Amount"] = 50;//DeviceId, curTime 말고는 아무값이나 집어넣음
        transaction["DueTime"] = 1531250611;
        transaction["RentalDevice"] = deviceId;
        transaction["ReturnTime"] = tSecond;
        transaction["Type"] = 3;
        transaction["UserAddress"] = userAddress;

        X9ECParameters ec = SecNamedCurves.GetByName("secp256k1");
        ECDomainParameters domainParams = new ECDomainParameters(ec.Curve, ec.G, ec.N, ec.H);
        BigInteger biPrivateKey = new BigInteger(privateKey);
        ECPrivateKeyParameters tmpPrivatePara = new ECPrivateKeyParameters(biPrivateKey, domainParams);


        ECDsaSigner privSigner = new ECDsaSigner();
        privSigner.Init(true, tmpPrivatePara);
        BigInteger[] signature = privSigner.GenerateSignature(Encoding.ASCII.GetBytes(returnRequestPacket["Body"].ToJson()));
        BigInteger r = signature[0];
        BigInteger s = signature[1];
        //디지털 서명을 하는 부분.
        header["EncryptR"] = bytesToHexString(r.ToByteArray());
        header["EncryptS"] = bytesToHexString(s.ToByteArray());
        header["PublicKey"] = publicKey;
        header["Type"] = "ReturnRequest";

        byte[] returnRequestPacketOutput = Encoding.ASCII.GetBytes(returnRequestPacket.ToJson());
        Debug.Log(returnRequestPacket.ToJson());

        TcpClient client = new TcpClient(nodeIP, nodePort);
        NetworkStream stream = client.GetStream();
        stream.Write(returnRequestPacketOutput, 0, returnRequestPacketOutput.Length);

        System.Array.Clear(buffer, 0, buffer.Length);
        stream.Read(buffer, 0, buffer.Length);
        JsonData returnResponsePacket = JsonMapper.ToObject(Encoding.ASCII.GetString(buffer));
        if (returnRequestPacket["Body"]["Response"].GetNatural() == 1)
        {
            returnText.text = "Return OK";
        }
        client.Close();
        return;
    }
}
