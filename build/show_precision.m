%clear,clc,close all


fi=linspace(-pi*10,pi*10,10001)';
fid=fopen('data.txt','w');
assert(fid>0);
fprintf(fid,'%.18g\n',fi);
fclose(fid);

ret=system('./cmdline <data.txt >out.txt && sync');
assert(0==ret);

out=load('out.txt');

ref=sin(fi);
semilogy(fi,abs(out-ref)	)

