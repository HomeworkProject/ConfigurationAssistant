{INCLUDES THE CODE THAT'S SUPPOSED TO USED ON THE GENGROUP_TAB}

procedure TfmMain.Edit_maskChange(Sender: TObject);
var
  s: String;
begin
  s := Edit_mask.Text;
  s := StringReplace(s, '%i%', '2', [rfReplaceAll]);
  s := StringReplace(s, '%a%', 'b', [rfReplaceAll]);
  s := StringReplace(s, '%A%', 'B', [rfReplaceAll]);
  Label_maskEx.Caption := 'Example: "' + s + '"';
end;

procedure TfmMain.FormCloseQuery(Sender: TObject; var CanClose: boolean);
var
  res: TModalResult;
begin
  if (FConfigModified = True) then begin
    if (not (MessageDlg('Unsaved config!', 'Closing now will not save your config, continue?', mtConfirmation, mbYesNo, 0) = mrYes)) then begin
      CanClose := False;
    end;
  end;
end;

procedure TfmMain.LabeledEdit_passMaskChange(Sender: TObject);
var
  s: String;
begin
  s := LabeledEdit_passMask.Text;
  s := StringReplace(s, '%i%', '2', [rfReplaceAll]);
  s := StringReplace(s, '%a%', 'b', [rfReplaceAll]);
  s := StringReplace(s, '%A%', 'B', [rfReplaceAll]);
  s := StringReplace(s, '%u%', 'user', [rfReplaceAll]);
  Label_maskPassEx.Caption := 'Example: "' + s + '"';
end;

procedure TfmMain.Button_startGenClick(Sender: TObject);
var
  I, I2, I3: Integer;
  s, u, p: String;
  uList: TStringList;
begin
  Memo_autoGenLog.Clear;
  if ((SpinEdit_fromClass.Value > SpinEdit_toClass.Value) or (ComboBox_fromSubClass.ItemIndex > ComboBox_toSubClass.ItemIndex)) then begin
     ShowMessage('Cannot run backwards ¯\_(ツ)_/¯');
     Exit;
  end;
  GroupBox_genGroups.Enabled := False;
  uList := TStringList.Create;
  uList.Delimiter := ',';
  uList.DelimitedText := Memo_users.Text;
  for I:=SpinEdit_fromClass.Value to SpinEdit_toClass.Value do begin
        for I2:=ComboBox_fromSubClass.ItemIndex to ComboBox_toSubClass.ItemIndex do begin
              s := Edit_mask.Text;
              s := StringReplace(s, '%i%', IntToStr(I), [rfReplaceAll]);
              s := StringReplace(s, '%a%', LowerCase(ComboBox_toSubClass.Items[I2]), [rfReplaceAll]);
              s := StringReplace(s, '%A%', ComboBox_toSubClass.Items[I2], [rfReplaceAll]);
              if (GroupExists(s)) then begin
                 if (CheckBox_genOverwrite.Checked) then begin
                    GroupCreate(s);
                    Memo_autoGenLog.Append(s + ' REPLACED');
                 end else begin
                       Memo_autoGenLog.Append(s + ' EXISTS');
                 end;
              end else begin
                  GroupCreate(s);
                  Memo_autoGenLog.Append(s + ' OK');
              end;
              for u in uList do begin
                    if (UserExists(s, u)) then begin
                       if (CheckBox_genOverwrite.Checked) then begin
                          UserCreate(s, u);
                          Memo_autoGenLog.Append(' ->' + s + '/' + u + ' REPLACED');
                       end else begin
                             Memo_autoGenLog.Append(' ->' + s + '/' + u + ' EXISTS');
                       end;
                    end else begin
                        UserCreate(s, u);
                        Memo_autoGenLog.Lines.Add(' ->' + s + '/' + u + ' OK');
                    end;
                    p := LabeledEdit_passMask.Text;
                    if (p <> '') then begin
                      p := StringReplace(p, '%i%', IntToStr(I), [rfReplaceAll]);
                      p := StringReplace(p, '%a%', LowerCase(ComboBox_toSubClass.Items[I2]), [rfReplaceAll]);
                      p := StringReplace(p, '%A%', ComboBox_toSubClass.Items[I2], [rfReplaceAll]);
                      p := StringReplace(p, '%u%', u, [rfReplaceAll]);
                    end;
                    Passwd(TJSONObject(FJSON.FindPath('groups.' + s + '.users.' + u)), 'default', p);
                    I3 := Memo_autoGenLog.Lines.Count - 1;
                    Memo_autoGenLog.Lines[I3] := Memo_autoGenLog.Lines[I3] + ' PASSWD OK';
              end;
        end;
  end;
  uList.Free;
  GroupBox_genGroups.Enabled := True;
end;

