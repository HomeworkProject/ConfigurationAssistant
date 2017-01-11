{INCLUDES THE CODE THAT'S SUPPOSED TO USED ON THE NETWORKCONF_TAB}

procedure TfmMain.Panel_tcpDblClick(Sender: TObject);
begin
  PageControl_main.ActivePage := TabSheet_networkConf;
end;

procedure TfmMain.CheckBox_enableSecTCPChange(Sender: TObject);
var
  b: Boolean;
begin
  b := CheckBox_enableSecTCP.Checked;
  LabeledEdit_sslCert.Enabled := b;
  LabeledEdit_sslCertKey.Enabled := b;
end;

procedure TfmMain.Button_applyNetworkingClick(Sender: TObject);
var
  jData: TJSONData;
  jTCP, jObj: TJSONObject;
begin
  jTCP := FJSON.Objects['tcp'];

  //Plaintext
  jData := jTCP.FindPath('plain');
  if (not Assigned(jData) or not (jData.JSONType = jtObject)) then begin
    jData := TJSONObject.Create;
    jTCP.Add('plain', TJSONObject(jData));
  end;
  jObj := TJSONObject(jData);
  jObj.Delete('enable');
  jObj.Add('enable', CheckBox_enablePlainTCP.Checked);
  jObj.Delete('port');
  jObj.Add('port', SpinEdit_plainTCPPort.Value);

  //Secure
  jData := jTCP.FindPath('ssl');
  if (not Assigned(jData) or not (jData.JSONType = jtObject)) then begin
    jData := TJSONObject.Create;
    jTCP.Add('ssl', TJSONObject(jData));
  end;
  jObj := TJSONObject(jData);
  jObj.Delete('enable');
  jObj.Add('enable', CheckBox_enableSecTCP.Checked);
  jObj.Delete('port');
  jObj.Add('port', SpinEdit_secTCPPort.Value);
  if (LabeledEdit_sslCert.Text <> '') then begin
     jObj.Delete('keystore');
     jObj.Add('keystore', LabeledEdit_sslCert.Text);
  end;
  if (LabeledEdit_sslCertKey.Text <> '') then begin
     jObj.Delete('password');
     jObj.Add('password', LabeledEdit_sslCertKey.Text);
  end;  

  FConfigModified := True;
end;
