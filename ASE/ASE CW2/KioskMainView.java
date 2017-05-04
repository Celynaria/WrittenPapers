package view;

import java.awt.BorderLayout;
import java.awt.Container;

import javax.swing.JFrame;
import javax.swing.JPanel;

@SuppressWarnings("serial")
public class KioskMainView extends JFrame{
	
	public KioskMainView (){
		
		setTitle("Taxi Kiosk");
		KioskListView j3 = new KioskListView();
		j3.setTitleLabel("TAXI GROUP");
		j3.setNumberLabel(3);
		j3.setButtonLabel("Taxi");
		KioskLogView j2 = new KioskLogView();
		j2.getTx().setText("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
		KioskListView j1 = new KioskListView();
		j1.setTitleLabel("PASSENGER GROUP");
		j1.setNumberLabel(3);
		j1.setButtonLabel("Queue");
		
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		setSize(500, 800);
		JPanel j = new JPanel();
		j.add(j1);
		j.add(j3);
		Container contentPane = getContentPane();
		contentPane.add(j,BorderLayout.NORTH);
		contentPane.add(j2, BorderLayout.SOUTH);
		pack();
		setVisible(true);
		
	}
	
 
}
