function betplatform
  fprintf("Available Serial Ports:\n");
  for i = serialportlist("available")
    fprintf("  %s\n",i);
  end
  fprintf("\n");
  %selected = input("Select a Serial Port: ", "s");

  %ground = serialport(selected, 115200);

  fig = uifigure('Name', 'BET Platform Control');
  fig.CloseRequestFcn = @(src,event)closerequest(src);


end