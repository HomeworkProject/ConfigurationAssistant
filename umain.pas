unit uMain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, ComCtrls,
  StdCtrls, ExtCtrls, Grids, Spin
  , uConfSearch
  , fpjson, jsonparser, jsonscanner
  , uPassWdGen
  ;

type

  { TfmMain }

  TfmMain = class(TForm)
    Button_applyNetworking: TButton;
    Button_startGen: TButton;
    Button_Generate: TButton;
    Button_apply: TButton;
    Button_crGroup: TButton;
    Button_crUser: TButton;
    Button_delGroup: TButton;
    Button_delUser: TButton;
    Button_saveConfig: TButton;
    Button_back: TButton;
    Button_exit: TButton;
    Button_openConfig: TButton;
    Button_browseConfigPath: TButton;
    CheckBox_enablePlainTCP: TCheckBox;
    CheckBox_enableFtTCP: TCheckBox;
    CheckBox_enableSecTCP: TCheckBox;
    CheckBox_genOverwrite: TCheckBox;
    CheckBox_autoApplyUpdate: TCheckBox;
    CheckBox_cleanuphwDB: TCheckBox;
    CheckBox_autoCheckUpdate: TCheckBox;
    CheckBox_ftTcp: TCheckBox;
    CheckBox_plainTcp: TCheckBox;
    CheckBox_secTcp: TCheckBox;
    CheckBox_setAdmin: TCheckBox;
    ComboBox_toSubClass: TComboBox;
    ComboBox_fromSubClass: TComboBox;
    ComboBox_configPath: TComboBox;
    Edit_authData: TLabeledEdit;
    Edit_authMethod: TLabeledEdit;
    Edit_configPath: TEdit;
    Edit_GUname: TLabeledEdit;
    Edit_mask: TLabeledEdit;
    Edit_pass: TLabeledEdit;
    GroupBox_plainTCP: TGroupBox;
    GroupBox_genGroups: TGroupBox;
    GroupBox_action: TGroupBox;
    GroupBox_ftTCP: TGroupBox;
    GroupBox_secTCP: TGroupBox;
    GroupBox_userInfo: TGroupBox;
    GroupBox_infoBalloon: TGroupBox;
    GroupBox_groups: TGroupBox;
    GroupBox_cleanup: TGroupBox;
    GroupBox_appInfo: TGroupBox;
    GroupBox_update: TGroupBox;
    iLabel_select: TLabel;
    LabeledEdit_ftTCPAddr: TLabeledEdit;
    LabeledEdit_sslCert: TLabeledEdit;
    LabeledEdit_plainTCPAddr: TLabeledEdit;
    LabeledEdit_secTCPAddr: TLabeledEdit;
    LabeledEdit_sslCertKey: TLabeledEdit;
    Label_iUList: TLabel;
    Label_iFromClass: TLabel;
    LabeledEdit_passMask: TLabeledEdit;
    Label_iFromSubClass: TLabel;
    Label_iToSubClass: TLabel;
    Label_iToClass: TLabel;
    Label_maskEx: TLabel;
    Label_iUsers: TLabel;
    Label_iGroups: TLabel;
    Label_groupCount: TLabel;
    Label_iUsersInGroups: TLabel;
    Label_maskPassEx: TLabel;
    Label_userCount: TLabel;
    Label_updateInterval: TLabel;
    Label_portFT: TLabel;
    Label_maxhwDBAge: TLabel;
    Label_portPlain: TLabel;
    Label_portSec: TLabel;
    Label_updateIntervalTU: TLabel;
    ListBox_users: TListBox;
    ListBox_groups: TListBox;
    ListBox_groupsnusers: TListBox;
    Memo_users: TMemo;
    Memo_autoGenLog: TMemo;
    Memo_info: TMemo;
    OpenDialog_confOpen: TOpenDialog;
    PageControl_main: TPageControl;
    Panel_tcp: TGroupBox;
    ProgressBar_searchConfigs: TProgressBar;
    RadioButton_randAlphaNUM: TRadioButton;
    RadioButton_createNew: TRadioButton;
    RadioButton_open: TRadioButton;
    RadioButton_detect: TRadioButton;
    SpinEdit_plainTCPPort: TSpinEdit;
    SpinEdit_fromClass: TSpinEdit;
    SpinEdit_ftTCPPort: TSpinEdit;
    SpinEdit_secTCPPort: TSpinEdit;
    SpinEdit_toClass: TSpinEdit;
    StaticText_genericInfo: TStaticText;
    TabSheet_networkConf: TTabSheet;
    TabSheet_genGroupsUsers: TTabSheet;
    TabSheet_editGroup: TTabSheet;
    TabSheet_config_generic: TTabSheet;
    TabSheet_selectFile: TTabSheet;
    Timer_info: TTimer;
    procedure Button_applyClick(Sender: TObject);
    procedure Button_applyNetworkingClick(Sender: TObject);
    procedure Button_backClick(Sender: TObject);
    procedure Button_browseConfigPathClick(Sender: TObject);
    procedure Button_crGroupClick(Sender: TObject);
    procedure Button_crUserClick(Sender: TObject);
    procedure Button_delGroupClick(Sender: TObject);
    procedure Button_delUserClick(Sender: TObject);
    procedure Button_GenerateClick(Sender: TObject);
    procedure Button_saveConfigClick(Sender: TObject);
    procedure Button_startGenClick(Sender: TObject);
    procedure CheckBox_enableSecTCPChange(Sender: TObject);
    procedure Edit_maskChange(Sender: TObject);
    procedure FormCloseQuery(Sender: TObject; var CanClose: boolean);
    procedure FormCreate(Sender: TObject);
    procedure GroupBox_groupsDblClick(Sender: TObject);
    procedure GroupBox_infoBalloonClick(Sender: TObject);
    procedure LabeledEdit_passMaskChange(Sender: TObject);
    procedure ListBox_groupsClick(Sender: TObject);
    procedure ListBox_groupsSelectionChange(Sender: TObject; User: boolean);
    procedure ListBox_usersSelectionChange(Sender: TObject; User: boolean);
    procedure PageControl_mainChange(Sender: TObject);
    procedure Panel_tcpDblClick(Sender: TObject);
    procedure RadioButton_openChange(Sender: TObject);
    procedure RadioButton_detectChange(Sender: TObject);
    procedure Button_openConfigClick(Sender: TObject);
    procedure Button_exitClick(Sender: TObject);
    procedure Timer_infoTimer(Sender: TObject);
  private
    FConfigPath: String;
    FJSON: TJSONObject;
    FConfigModified: Boolean;
    FInfoList: TStringList;
    procedure AddInfo(s: String);
    procedure GroupCreate(gName: String);
    function GroupExists(gName: String): Boolean;
    function NextInfo: Boolean;
    procedure OnConfSearchDone(confSearch: TConfSearch);
    procedure loadConf(path: String);
    procedure Passwd(user: TJSONObject; method, password: String);
    function reloadFromJSON: Boolean;
    procedure UserCreate(gName, uName: String);
    function UserExists(gName, uName: String): Boolean;
  public
  end;

var
  fmMain: TfmMain;

implementation

{$R *.lfm}

{ TfmMain }

procedure TfmMain.OnConfSearchDone(confSearch: TConfSearch);
var
  I: Integer;
  s: String;
begin
  //ShowMessage('Test');
  for I:=0 to (confSearch.getResults.Count - 1) do begin
      s := StringReplace(confSearch.getResults.Strings[I], GetCurrentDir, '.', [rfReplaceAll{$IFNDEF UNIX}, rfIgnoreCase{$ENDIF}]);
    ComboBox_configPath.Items.Add(s);
  end;
  ProgressBar_searchConfigs.Visible := False;
  RadioButton_detect.Enabled := True;
  RadioButton_open.Enabled := True;
  if (ComboBox_configPath.Items.Count <= 0) then begin
    ComboBox_configPath.Enabled := False;
    ComboBox_configPath.Text := 'No possible config found!';
  end else begin
    ComboBox_configPath.ItemIndex := 0;
    ComboBox_configPath.Enabled := True;
  end;
end;

procedure TfmMain.RadioButton_openChange(Sender: TObject);
begin
  if (RadioButton_open.Checked) then begin
    //Enable this cat due to switch
    RadioButton_open.Enabled := True;
    Edit_configPath.Enabled := True;
    Button_browseConfigPath.Enabled := True;
  end else begin
      //Disable this cat due to switch
      RadioButton_open.Enabled := False;
      Edit_configPath.Enabled := False;
      Button_browseConfigPath.Enabled := False;
  end;
end;

procedure TfmMain.FormCreate(Sender: TObject);
begin
  Height := 400;
  Width := 600;
  BorderStyle := bsSingle;

  FInfoList := TStringList.Create;
  GroupBox_infoBalloon.Left := Width - GroupBox_infoBalloon.Width;

  PageControl_main.ActivePage := TabSheet_selectFile;
  RadioButton_openChange(Self);
  RadioButton_detectChange(Self);

  LabeledEdit_sslCert.Caption := 'Keystore path (empty for unchanged)';
  LabeledEdit_sslCert.Text := '';
  LabeledEdit_sslCertKey.Caption := 'Keystore password (empty for unchanged)';
  LabeledEdit_sslCertKey.Caption := '';
end;

procedure TfmMain.Button_backClick(Sender: TObject);
begin
  PageControl_main.ActivePage := TabSheet_config_generic;
  PageControl_mainChange(Button_back);
end;

procedure TfmMain.Button_browseConfigPathClick(Sender: TObject);
var
  s: String;
begin
  with OpenDialog_confOpen do begin
        DefaultExt := 'json';
        InitialDir := GetCurrentDir;
        Options := [ofEnableSizing, ofHideReadOnly, ofViewDetail, ofPathMustExist];
        Title := 'Select configuration file';
        if (Execute) then s := FileName
        else s := '';
  end;
  s := StringReplace(s, GetCurrentDir, '.', [rfReplaceAll{$IFNDEF UNIX}, rfIgnoreCase{$ENDIF}]);
  Edit_configPath.Text := s;
end;

procedure TfmMain.Button_saveConfigClick(Sender: TObject);
var
  f: TextFile;
begin
  AssignFile(f, FConfigPath);
  Rewrite(f);
  Write(f, FJSON.FormatJSON());
  Flush(f);
  CloseFile(f);
  AddInfo('Configuration saved' + LineEnding + TimeToStr(now));
  FConfigModified := False;
end;

procedure TfmMain.GroupBox_groupsDblClick(Sender: TObject);
begin
  PageControl_main.ActivePage := TabSheet_editGroup;
  PageControl_mainChange(GroupBox_groups);
  ListBox_groupsSelectionChange(Self, False);
end;

procedure TfmMain.RadioButton_detectChange(Sender: TObject);
var
  confSearch: TConfSearch;
begin
  if (RadioButton_detect.Checked) then begin
    //Disable this cat for search
    ComboBox_configPath.Clear;
    ComboBox_configPath.Enabled := False;
    ComboBox_configPath.Text := 'Searching...';
    ProgressBar_searchConfigs.Visible := True;
    RadioButton_detect.Enabled := False;

    //Start search
    confSearch := TConfSearch.Create;
    confSearch.SetOnDone(@OnConfSearchDone);
    confSearch.FreeOnTerminate := True;
    confSearch.Start;
  end else begin
    ComboBox_configPath.Enabled := False;
  end;
end;

procedure TfmMain.Button_openConfigClick(Sender: TObject);
begin
  if (RadioButton_open.Checked) then begin
    loadConf(Edit_configPath.Text);
  end else if (RadioButton_detect.Checked) then begin
    loadConf(ComboBox_configPath.Items[ComboBox_configPath.ItemIndex]);
  end else if (RadioButton_createNew.Checked) then begin
    loadConf('.');
  end else begin
      {@ERRORCODE = #0001}
      ShowMessage('Unable to determine selection! Please report this error - Code #0001');
  end;
end;

procedure TfmMain.Button_exitClick(Sender: TObject);
begin
  Close;
end;

procedure TfmMain.PageControl_mainChange(Sender: TObject);
begin
  Button_back.Visible := False;
  if (PageControl_main.ActivePage = TabSheet_editGroup) then
     Button_back.Visible := True;
  if (PageControl_main.ActivePage = TabSheet_networkConf) then
     Button_back.Visible := True;
  if (PageControl_main.ActivePage = TabSheet_genGroupsUsers) then
     Button_back.Visible := True; 
end;

{$INCLUDE 'umain_loadconf.inc'}
{$INCLUDE 'umain_groupedit.inc'}
{$INCLUDE 'umain_tools.inc'}
{$INCLUDE 'umain_gengroups.inc'}
{$INCLUDE 'umain_networkconf.inc'}

end.

