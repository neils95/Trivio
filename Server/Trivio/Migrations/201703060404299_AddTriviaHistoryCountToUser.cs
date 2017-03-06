namespace Trivio.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddTriviaHistoryCountToUser : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.Users", "TriviaHistoryCount", c => c.Int(nullable: false));
        }
        
        public override void Down()
        {
            DropColumn("dbo.Users", "TriviaHistoryCount");
        }
    }
}
