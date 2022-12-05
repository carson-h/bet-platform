function handlerequest(~,~,pan,ser,dist_gauge,pitch_gauge,roll_gauge)
  
  %% Data Acquisition
  write(ser, 224, "uint8"); % 224
  wait = 0;
  while (ser.NumBytesAvailable < 64*4)
    pause(0.001);
    if (wait > 100)
      return
    end
    wait = wait + 1;
  end

  thermo = read(ser,64,"single");
  while (ser.NumBytesAvailable < 1*4)
    pause(0.001);
  end
  dist = read(ser,1,"single");
  while (ser.NumBytesAvailable < 1*4)
    pause(0.001);
  end
  ori = read(ser,2,"single");
  while (ser.NumBytesAvailable < 1)
    pause(0.001);
  end
  [~] = read(ser,1,"uint8");

  %% Data Display
  
  delete(pan.Children())
  res = interp2(reshape(thermo,8,8),4,"cubic");
  hm = heatmap(pan, res);
  hm.Colormap = spring;
  hm.GridVisible = 'off';
  hm.PositionConstraint = 'outerposition';
  hm.Position = [0 0 1 1];
  hm.OuterPosition = [0 0 1 1];
  hm.CellLabelColor = 'None';
  hm.XDisplayLabels = nan(size(res,1),1);
  hm.YDisplayLabels = nan(size(res,1),1);
  

  dist_gauge.Value = dist;
  pitch_gauge.Value = ori(1);
  roll_gauge.Value = ori(2);

end