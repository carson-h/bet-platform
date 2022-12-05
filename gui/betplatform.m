function betplatform
  %% Ground Station Connection
  fprintf("Available Serial Ports:\n");
  for i = serialportlist("available")
    fprintf("  %s\n",i);
  end
  fprintf("\n");
  selected = input("Select a Serial Port: ", "s");

  ser = serialport(selected, 115200, "Parity",'odd','DataBits',8,'StopBits',1,'ByteOrder','little-endian');

  %% Refresh
  
  tmr = timer('ExecutionMode', 'fixedRate','BusyMode', 'queue');
  
  %% GUI

  fig = uifigure('Name', 'BET Platform Control');
  fig.CloseRequestFcn = {@closerequest, ser, tmr};

  g = uigridlayout(fig);
  g.ColumnWidth = {'1x', 300};
  g.RowHeight = {'1x', '1x'};

  %% Configuration Panel

  config_panel = uipanel(g, "Title", "Configuration");
  config_panel.Layout.Row = 1;
  config_panel.Layout.Column = 2;

  g_config = uigridlayout(config_panel);
  g_config.ColumnWidth = {'fit', '1x', 25};
  g_config.RowHeight = {'1x', '1x', '1x'};

  freq_label = uilabel(g_config, 'Text','Frequency');
  freq_label.Layout.Column = 1;
  freq_label.Layout.Row = 1;

  freq_field = uieditfield(g_config, "numeric", 'Value', 4);
  freq_field.Layout.Column = 2;
  freq_field.Layout.Row = 1;

  freq_label_u = uilabel(g_config, 'Text', 'Hz');
  freq_label_u.Layout.Column = 3;
  freq_label_u.Layout.Row = 1;

  pitch_label_c = uilabel(g_config, 'Text','Pitch');
  pitch_label_c.Layout.Column = 1;
  pitch_label_c.Layout.Row = 2;

  pitch_field_c = uieditfield(g_config, "numeric", 'Value', 0);
  pitch_field_c.Layout.Column = 2;
  pitch_field_c.Layout.Row = 2;

  pitch_label_c_u = uilabel(g_config, 'Text', 'deg');
  pitch_label_c_u.Layout.Column = 3;
  pitch_label_c_u.Layout.Row = 2;
  
  roll_label_c = uilabel(g_config, 'Text','Roll');
  roll_label_c.Layout.Column = 1;
  roll_label_c.Layout.Row = 3;

  roll_field_c = uieditfield(g_config, "numeric", 'Value', 0);
  roll_field_c.Layout.Column = 2;
  roll_field_c.Layout.Row = 3;

  roll_label_c_u = uilabel(g_config, 'Text', 'deg');
  roll_label_c_u.Layout.Column = 3;
  roll_label_c_u.Layout.Row = 3;


  %% Data Panel

  data_panel = uipanel(g, "Title", "Status");
  data_panel.Layout.Row = 2;
  data_panel.Layout.Column = 2;

  g_data = uigridlayout(data_panel);
  g_data.ColumnWidth = {'fit', '1x', 25};
  g_data.RowHeight = {'1x', '1x', '1x'};

  dist_label = uilabel(g_data, 'Text', 'Distance');
  dist_label.Layout.Column = 1;
  dist_label.Layout.Row = 1;

  dist_label_u = uilabel(g_data, 'Text', 'm');
  dist_label_u.Layout.Column = 3;
  dist_label_u.Layout.Row = 1;

  dist_gauge = uigauge(g_data, 'linear');
  dist_gauge.Layout.Column = 2;
  dist_gauge.Layout.Row = 1;
  dist_gauge.Limits = [0 4];
  dist_gauge.MajorTicks = [0 1 2 3 4];

  pitch_label = uilabel(g_data, 'Text', 'Pitch');
  pitch_label.Layout.Column = 1;
  pitch_label.Layout.Row = 2;

  pitch_label_u = uilabel(g_data, 'Text', 'deg');
  pitch_label_u.Layout.Column = 3;
  pitch_label_u.Layout.Row = 2;

  pitch_gauge = uigauge(g_data, 'linear');
  pitch_gauge.Layout.Column = 2;
  pitch_gauge.Layout.Row = 2;
  pitch_gauge.Limits = [-30, 30];
  pitch_gauge.MajorTicks = [-30 -15 0 15 30];

  roll_label = uilabel(g_data, 'Text', 'Roll');
  roll_label.Layout.Column = 1;
  roll_label.Layout.Row = 3;

  roll_gauge = uigauge(g_data, 'linear');
  roll_gauge.Layout.Column = 2;
  roll_gauge.Layout.Row = 3;
  roll_gauge.Limits = [-30, 30];
  roll_gauge.MajorTicks = [-30 -15 0 15 30];

  roll_label_u = uilabel(g_data, 'Text', 'deg');
  roll_label_u.Layout.Column = 3;
  roll_label_u.Layout.Row = 3;

  %% Thermal Image

  pan = uipanel(g);
  pan.Layout.Row = [1 2];
  pan.Layout.Column = 1;
  pan.AutoResizeChildren = 'off';

  tmr.Period = 1/freq_field.Value;
  tmr.TimerFcn={@handlerequest,pan,ser,dist_gauge,pitch_gauge,roll_gauge};
  start(tmr);

end