function filtered = kalman1d(q, r, p, x ,k, data)

  # Initialize
  x = data(1);
  filtered = x;

  # Go through samples
  for i=2:size(data,1)

    # Prediction Update
    p = p + q;

    # Measurement Update
    k = p / (p + r);
    x = x + k*(data(i) -x);
    p = (1-k)*p;
    
    # Append filtered value to result vector
    filtered = horzcat(filtered, x);
  end

endfunction
