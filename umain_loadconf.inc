{THIS FILE CONTAINS THE CODE THAT'S SUPPOSED TO LOAD THE CONFIG}

procedure TfmMain.loadConf(path: String);
var
  f: File;
  s: String;
  parser: TJSONParser;
  jData: TJSONData;
  jObj: TJSONObject;
begin
  if (Assigned(FJSON)) then
     FJSON.Free;
  FJSON := nil;
  FConfigPath := '';
  if (Length(path) = 1) then begin
    //Supposed to create new config
    path := 'newConfig.json';
    AssignFile(f, path);
    Rewrite(f);
    CloseFile(f);
  end;

  if (not FileExists(path)) then begin
    ShowMessage('Cannot open file (404): ' + path);
    Exit;
  end;
  s := FileUtil.ReadFileToString(path);
  try
    parser := TJSONParser.Create(s, [joUTF8, joComments]);
    jData := parser.Parse;
    if (not Assigned(jData)) then begin
      jObj := TJSONObject.Create;
    end else begin
    if (not (jData.JSONType = jtObject)) then begin
      parser.Free;
      ShowMessage('Cannot parse file: Not a json object!');
      Exit;
    end;
    jObj := TJSONObject(jData);
    end;
  except
    on e: Exception do begin
      if (Assigned(parser)) then
         parser.Free;
      ShowMessage('Cannot parse file: ' + e.Message);
    end;
  end;
  FJSON := jObj;

  if (reloadFromJSON) then begin
    PageControl_main.ActivePage := TabSheet_config_generic;
    TabSheet_config_generic.Enabled := True;
  end else begin
      PageControl_main.ActivePage := TabSheet_selectFile;
      TabSheet_config_generic.Enabled := False;
  end;
end;

function TfmMain.reloadFromJSON: Boolean;
var
  jData: TJSONData;
  jObj: TJSONObject;
  b: Boolean;
  i: Integer;
begin
  try
    //Plain TCP
    b:= False;
    jData := FJSON.GetPath('tcp.plain');
    if (Assigned(jData)) then begin
      if (jData.JSONType = jtObject) then begin
        jObj := TJSONObject(jData);
        if (jObj.Get('enable', False)) then
           b := True;
        i := jObj.Get('port', 0);
      end;
    end;
    CheckBox_plainTcp.Checked := b;
    if (i<>0) then
       Label_portPlain.Caption := IntToStr(i)
    else
      Label_portPlain.Caption := '-';

        //Plain TCP
    b:= False;
    jData := FJSON.GetPath('tcp.ssl');
    if (Assigned(jData)) then begin
      if (jData.JSONType = jtObject) then begin
        jObj := TJSONObject(jData);
        if (jObj.Get('enable', False)) then
           b := True;
        i := jObj.Get('port', 0);
      end;
    end;
    CheckBox_secTcp.Checked := b;
    if (i<>0) then
       Label_portSec.Caption := IntToStr(i)
    else
      Label_portSec.Caption := '-';

    //FT TCP
    b:= False;
    jData := FJSON.GetPath('tcp.ft');
    if (Assigned(jData)) then begin
      if (jData.JSONType = jtObject) then begin
        jObj := TJSONObject(jData);
        if (jObj.Get('enable', False)) then
           b := True;
        i := jObj.Get('port', 0);
      end;
    end;
    CheckBox_ftTcp.Checked := b;
    if (i<>0) then
       Label_portFT.Caption := IntToStr(i)
    else
      Label_portFT.Caption := '-';

  except
  on e: Exception do begin
     ShowMessage('Cannot read config: ' + e.Message);
     Result := False;
  end;
end;
end;

end.
