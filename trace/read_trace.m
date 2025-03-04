function z = read_trace(filename) % z의 i번째 열에 i번째 전력 파형을 로딩
fid = fopen(filename, 'r');

header = fread(fid, 2, 'int');
trLen = header(1);
trNum = header(2);

for i=1:trNum % z의 크기 trLen + trNum
    z(:, i) = fread(fid, trLen, "float");

end

fclose(fid);

end

% 펑션 안의 데이터는 펑션에서 나오면 메모리에 남지 않음