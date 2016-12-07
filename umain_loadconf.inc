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
    PageControl_mainChange(Self);
    TabSheet_config_generic.Enabled := True;
    FConfigPath := path;
  end else begin
      PageControl_main.ActivePage := TabSheet_selectFile;
      PageControl_mainChange(Self);
      TabSheet_config_generic.Enabled := False;
  end;
end;

function TfmMain.reloadFromJSON: Boolean;
var
  jData, jData2: TJSONData;
  jObj, jObj2: TJSONObject;
  b, b2: Boolean;
  i, i2, i3, i4: Integer;
  s, s2: String;
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
    b := False;
    jData := FJSON.FindPath('tcp.ft');
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

    //HW-DB-Cleanup
    b := False;
    jData := FJSON.FindPath('cleanup.hw_database');
    if (Assigned(jData)) then begin
      if (jData.JSONType = jtObject) then begin
        jObj := TJSONObject(jData);
        if (jObj.Get('enable', False)) then
           b := True;
        i := jObj.Get('maxAgeDays', 0);
      end;
    end;
    CheckBox_cleanuphwDB.Checked := b;
    if (i<>0) then
       Label_maxhwDBAge.Caption := '>' + IntToStr(i) + 'd'
    else
      Label_maxhwDBAge.Caption := '-';

    //Automatic updates
    b := False;
    b2 := True;
    jData := FJSON.FindPath('update');
    if (Assigned(jData)) then begin
      if (jData.JSONType = jtObject) then begin
        jObj := TJSONObject(jData);
        if (jObj.Get('enable', False)) then
           b := True;
        if (not jObj.Get('upgrade', True)) then
           b2 := False;
        i := jObj.Get('interval', 0);
        s := jObj.Get('intervalTimeUnit', 'SECONDS');
      end;
    end;
    CheckBox_autoCheckUpdate.Checked := b;
    CheckBox_autoApplyUpdate.Checked := b2;
    if (i<>0) then begin
       Label_updateInterval.Caption := IntToStr(i);
       Label_updateIntervalTU.Caption := s;
    end else begin
      Label_updateInterval.Caption := '-';
      Label_updateIntervalTU.Caption := '-';
    end;

    //Groups+Users
    ListBox_groupsnusers.Clear;
    i3 := 0;
    i4 := 0;
    jData := FJSON.FindPath('groups');
    if (jData.JSONType = jtObject) then begin
      jObj := TJSONObject(jData);
      for i:=0 to (jObj.Count-1) do begin
          s := jObj.Names[i];
          jData2 := jObj.FindPath(s + '.users');
          if (Assigned(jData2)) then
          if (jData2.JSONType = jtObject) then begin
            if (jData2.JSONType = jtObject) then begin
              ListBox_groupsnusers.Items.Add(s);
              ListBox_groups.Items.Add(s);
              Inc(i3);
              jObj2 := TJSONObject(jData2);
              for i2:=0 to (jObj2.Count-1) do begin
                  ListBox_groupsnusers.Items.Add('  ->' + jObj2.Names[i2]);
                  Inc(i4);
              end;
            end;
          end;
      end;
    end;
    Label_groupCount.Caption := IntToStr(i3) + ' group(s)';
    Label_userCount.Caption := IntToStr(i4) + ' user(s)';

  except
  on e: Exception do begin
     ShowMessage('Cannot read config: ' + e.Message);
     Result := False;
  end;
end;
end;

