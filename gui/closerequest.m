function closerequest(fig)
  selection = uiconfirm(fig,'Close the ground station connection?','Confirmation');
        
  switch selection
    case 'OK'
      %flush(ground);
      %ground = [];
      delete(fig);
    case 'Cancel'
      return
  end
end