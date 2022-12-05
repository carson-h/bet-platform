function closerequest(fig,~,ground,tmr)
  selection = uiconfirm(fig,'Close the ground station connection?','Confirmation');
        
  switch selection
    case 'OK'
      flush(ground);
      delete(ground);
      stop(tmr);
      delete(fig);
    case 'Cancel'
      return
  end
end