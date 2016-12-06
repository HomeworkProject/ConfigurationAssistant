unit uMain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, ComCtrls,
  StdCtrls, ExtCtrls
  , uConfSearch
  , fpjson, jsonparser, jsonscanner
  ;

type

  { TfmMain }

  TfmMain = class(TForm)
    Button_exit: TButton;
    Button_openConfig: TButton;
    Button_browseConfigPath: TButton;
    CheckBox_ftTcp: TCheckBox;
    CheckBox_plainTcp: TCheckBox;
    CheckBox_secTcp: TCheckBox;
    ComboBox_configPath: TComboBox;
    Edit_configPath: TEdit;
    GroupBox_appInfo: TGroupBox;
    iLabel_select: TLabel;
    Label_portPlain: TLabel;
    Label_portSec: TLabel;
    Label_portFT: TLabel;
    PageControl_main: TPageControl;
    Panel_tcp: TPanel;
    ProgressBar_searchConfigs: TProgressBar;
    RadioButton_createNew: TRadioButton;
    RadioButton_open: TRadioButton;
    RadioButton_detect: TRadioButton;
    StaticText_genericInfo: TStaticText;
    TabSheet_config_generic: TTabSheet;
    TabSheet_selectFile: TTabSheet;
    procedure FormCreate(Sender: TObject);
    procedure RadioButton_openChange(Sender: TObject);
    procedure RadioButton_detectChange(Sender: TObject);
    procedure Button_openConfigClick(Sender: TObject);
    procedure Button_exitClick(Sender: TObject);
  private
    FConfigPath: String;
    FJSON: TJSONObject;
    procedure OnConfSearchDone(confSearch: TConfSearch);
    procedure loadConf(path: String);
    function reloadFromJSON: Boolean;
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
      s := StringReplace(confSearch.getResults.Strings[I], GetCurrentDir, '.', [rfReplaceAll{IFNDEF UNIX}, rfIgnoreCase{ENDIF}]);
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

  PageControl_main.ActivePage := TabSheet_selectFile;
  RadioButton_openChange(Self);
  RadioButton_detectChange(Self);
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

{$INCLUDE 'umain_loadconf.inc'}

end.

