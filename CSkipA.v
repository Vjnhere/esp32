module ha(a,b, s,c);
    input a,b;
    input s,c;
        assign s = a^b;
        assign c = a&b;
endmodule

module fa_ha(a,b,c, sum,carry);
    input a,b,c;
    output sum,carry;
        wire s,c1,c2;
        ha ha1 (a,b,s1,c1);
        ha ha2 (s1,c,sum,c2);
        assign carry = c1|c2;
endmodule

module rca(a,b, c, sum, carry);
    input [3:0] a,b;
    input c;
    output [3:0] sum;
    output carry;
        wire c1,c2,c3;
        fa_ha fa0 (a[0],b[0],c,sum[0],c1);
        fa_ha fa1 (a[1],b[1],c1,sum[1],c2);
        fa_ha fa2 (a[2],b[2],c2,sum[2],c3);
        fa_ha fa3 (a[3],b[3],c3,sum[3],carry);
endmodule

module propagate_g(a,b, p, bp);
    input [3:0] a,b;
    output [3:0] p;
    output bp;
        assign p = a^b;
        assign bp = &p;
endmodule

module mux2x1(
input x,y,s,
output cout
);
assign cout = (s?x:y);
endmodule

module csa(
input [3:0] a,b,
input cin,
output[3:0]sum,
output cout
);
wire [3:0]p;
wire c0;
wire bp;
rca rca (a[3:0],b[3:0],cin,sum[3:0],co);
propagate_g p1(a,b,p,bp);
mux2x1 m1 (c0,cin,bp,cout);
endmodule

module csa_tb_v;
        // Inputs
        reg [3:0] a;
        reg [3:0] b;
        reg cin;
        // Outputs
        wire [3:0] sum;
        wire cout;
        // Instantiate the Unit Under Test (UUT)
        csa uut (
                .a(a),
                .b(b),
                .cin(cin),
                .sum(sum),
                .cout(cout)
        );
        initial begin
                a = 4'b0001; b = 4'b0001; cin = 0; #100;
                a = 4'b0111; b = 4'b0001; cin = 0; #100;
                a = 4'b0101; b = 4'b0101; cin = 0; #100;
                a = 4'b1111; b = 4'b1111; cin = 0; #100;
                a = 4'b1100; b = 4'b1101; cin = 0; #100;
        end
endmodule
