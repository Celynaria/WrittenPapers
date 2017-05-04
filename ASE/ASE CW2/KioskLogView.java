package view;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;

@SuppressWarnings("serial")
public class KioskLogView extends JPanel{
	
	private JTextArea tx = new JTextArea();
	private JLabel title = new JLabel("Log Area");
	
	public KioskLogView(){
		
		add("title", title);
		add("Log", tx);
		
	}

	public JTextArea getTx() {
		return tx;
	}

	public void setTx(JTextArea tx) {
		this.tx = tx;
	}

	public JLabel getTitle() {
		return title;
	}

	public void setTitle(JLabel title) {
		this.title = title;
	}
	
	

}
